#!/bin/bash
homeworkName=HW5
binaryName=hw5
testcasePool="ibm01 ibm02 ibm03 ibm04"
timeLimit=600

root=$( pwd )
outputDir=$root/output
studentDir=$root/student
testcaseDir=$root/testcases
verifyBin=$root/verifier/verify
chmod 744 $verifyBin
csvFile=$root/${homeworkName}_grade.csv

function timeout()
{
	perl -e 'alarm shift; exec @ARGV' "$@";
}

function executionCmd () 
{
	local argv="$testcaseDir/$1.modified.txt $outputDir/$1.result"
	local log=$( ( time -p timeout $timeLimit ./$binaryName $argv ) 2>&1 > /dev/null )
	if [[ $log =~ "perl -e" ]]; then
		echo "TLE"
	else
		echo "$( echo "$log" | grep real | cut -d ' ' -f 2 )"
	fi
}

function verifyCmd ()
{
	local argv="$testcaseDir/$1.modified.txt $outputDir/$1.result"
	local log=$( $verifyBin $argv | cat )
	if [[ $log =~ "# RESULT #" ]]; then
		if [[ $log =~ "# of unconnected net because of pin: 0" ]]; then
			if [[ $log =~ "# of unconnected net because of wire: 0" ]]; then
				if [[ $log =~ "# of net has duplicate wires: 0" ]]; then
					local overflow=$( echo "$log" | grep '# of overflow:' | cut -d ' ' -f 4 )
					local wirelength=$( echo "$log" | grep 'Total WireLength:' | cut -d ' ' -f 3 )
					echo "success $overflow $wirelength"
				else
					echo "$1 has duplicate wires."
				fi
			else
				echo "$1 has floating wires."
			fi
		else
			echo "$1 has floating pins."
		fi
	elif [[ $log =~ "same" ]]; then
		echo "$1 has wrong net ordering."
	else
		echo "$1 has something error."
	fi
}

echo "|------------------------------------------------|"
echo "|                                                |"
echo "|    This script is used for PDA $homeworkName grading.    |"
echo "|                                                |"
echo "|------------------------------------------------|"

csvTitle="student id"
for testcase in $testcasePool; do
	csvTitle="$csvTitle, $testcase overflow, $testcase wirelength, $testcase runtime"
done
echo "$csvTitle, status" > $csvFile

cd $studentDir/
for studentId in *; do
	if [[ -d $studentId ]]; then 
		printf "grading on %s:\n" $studentId
		cd $studentId/

		rm -rf $( find . -mindepth 1 -maxdepth 1 -type d )
		tar -zxvf CS6135_${homeworkName}_$studentId.tar.gz > /dev/null
		find . -name ".*" -delete
		cd $homeworkName/

		cd src/
		make clean > /dev/null
		rm -rf ./bin/*
		make > /dev/null
		cd ..

		rm -rf $outputDir/*
		cd bin/
		csvContent="$studentId"
		statusList=""
		printf "%10s | %10s | %10s | %10s | %s\n" testcase overflow wirelength runtime status
		for testcase in $testcasePool; do
			printf "%10s | " $testcase

			runtime=$( executionCmd $testcase )
			overflow=fail
			wirelength=fail
			if [[ $runtime != "TLE" ]]; then
				status=$( verifyCmd $testcase )

				if [[ $status =~ "success" ]]; then
					overflow=$( echo "$status" | cut -d ' ' -f 2 )
					wirelength=$( echo "$status" | cut -d ' ' -f 3 )
					status=success
				else
					statusList="$statusList $status"
				fi
			else
				status="$testcase failed."
				statusList="$statusList $status"
			fi

			printf "%10s | %10s | %10s | %s\n" $overflow $wirelength $runtime "$status"
			csvContent="$csvContent, $overflow, $wirelength, $runtime"
		done
		cd ..

		echo "$csvContent, $statusList" >> $csvFile
		cd $studentDir/
	fi
done

echo "|-----------------------------------------------------|"
echo "|                                                     |"
echo "|    Successfully generate grades to ${homeworkName}_grade.csv    |"
echo "|                                                     |"
echo "|-----------------------------------------------------|"
