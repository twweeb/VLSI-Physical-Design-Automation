#!/bin/bash
homeworkName=HW3
binaryName=hw3
testcasePool="n100 n200 n300"
ratioPool="0.15 0.1"
timeLimit=1200

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
	local argv="$testcaseDir/$1.hardblocks $testcaseDir/$1.nets $testcaseDir/$1.pl $outputDir/$1.floorplan $2"
	local log=$( ( time -p timeout $timeLimit ./$binaryName $argv ) 2>&1 > /dev/null )
	if [[ $log =~ "perl -e" ]]; then
		echo "TLE"
	else
		echo "$( echo "$log" | grep real | cut -d ' ' -f 2 )"
	fi
}

function verifyCmd ()
{
	local argv="$testcaseDir/$1.hardblocks $testcaseDir/$1.nets $testcaseDir/$1.pl $outputDir/$1.floorplan $2"
	local log=$( $verifyBin $argv | cat )
	if [[ $log =~ "Usage:" ]]; then
		echo "Verifier fails in $1."
	elif [[ $log =~ "Error: Checking overlap" ]]; then
		echo "$1 has some overlapping cells."
	elif [[ $log =~ "Error: Checking fixed-outline" ]]; then
		echo "$1 has some cells out of outline."
	elif [[ $log =~ "Error: Wirelength" ]]; then
		echo "$1 has wrong wirelength."
	elif [[ $log =~ "OK!!" ]]; then
		echo "success"
	else
		echo "$1 has something wrong."
	fi
}

echo "|------------------------------------------------|"
echo "|                                                |"
echo "|    This script is used for PDA $homeworkName grading.    |"
echo "|                                                |"
echo "|------------------------------------------------|"

csvTitle="student id"
for ratio in $ratioPool; do
	for testcase in $testcasePool; do
		csvTitle="$csvTitle, $testcase-$ratio wirelength, $testcase-$ratio runtime"
	done
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
		printf "  testcase |      ratio | wirelength |    runtime | status\n"
		for ratio in $ratioPool; do
			for testcase in $testcasePool; do
				printf "%10s | %10s | " $testcase $ratio

				runtime=$( executionCmd $testcase $ratio )
				if [[ $runtime != "TLE" ]]; then
					status=$( verifyCmd $testcase $ratio )

					if [[ $status == "success" ]]; then
						wirelength=$( cat $outputDir/$testcase.floorplan | grep Wirelength | cut -d ' ' -f 2 )
					else
						wirelength=fail
						statusList="$statusList $status"
					fi
				else
					wirelength=fail
					status="$testcase failed."
					statusList="$statusList $status"
				fi

				printf "%10s | %10s | %s\n" $wirelength $runtime "$status"
				csvContent="$csvContent, $wirelength, $runtime"
			done
		done
		cd ..

		echo "$csvContent,$statusList" >> $csvFile
		cd $studentDir/
	fi
done

echo "|-----------------------------------------------------|"
echo "|                                                     |"
echo "|    Successfully generate grades to ${homeworkName}_grade.csv    |"
echo "|                                                     |"
echo "|-----------------------------------------------------|"
