#!/bin/bash
homeworkName=HW4
binaryName=hw4
testcasePool="ibm01 ibm02 ibm05 ibm07 ibm08 ibm09"
timeLimit=1200

root=$( pwd )
studentDir=$root/student
testcaseDir=$root/testcases
checkerFile=$root/checker/check_density_target.pl
csvFile=$root/${homeworkName}_grade.csv

function timeout()
{
	perl -e 'alarm shift; exec @ARGV' "$@";
}

function executionCmd () 
{
	local log=$( ( time -p timeout $timeLimit ./$binaryName -aux $testcaseDir/$1.aux ) 2>&1 > /dev/null )
	if [[ $log =~ "perl -e" ]]; then
		echo "TLE"
	else
		echo "$log"
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

		make clean > /dev/null
		make > /dev/null

		rm -rf output/
		mkdir output/
		csvContent="$studentId"
		statusList=""
		printf "%10s | %15s | %15s | %10s | %s\n" testcase overflow wirelength runtime status
		for testcase in $testcasePool; do
			printf "%10s | " $testcase
			if [[ $testcase == "ibm01" ]]; then
				name="$testcase-cu85"
			elif [[ $testcase == "ibm05" ]]; then
				name="$testcase"
			else
				name="$testcase-cu90"
			fi

			result=$( executionCmd $testcase/$name )
			isLegal=fail
			overflow=fail
			wirelength=fail
			runtime=TLE
			if [[ $result != "TLE" ]]; then
				isLegal="$( echo "$result" | grep HW4-result: | cut -d ' ' -f 2 )"
				wirelength="$( echo "$result" | grep HW4-result: | cut -d ' ' -f 3 | cut -d '.' -f 1 )"
				runtime="$( echo "$result" | grep real | cut -d ' ' -f 2 )"

				if [[ $isLegal == "1" ]]; then
					overflow=0
					status=success
				elif [[ $isLegal == "0" ]]; then
					nodes=$testcaseDir/$testcase/$testcase.nodes
					pl=output/$name.gp.pl
					scl=$testcaseDir/$testcase/$name.scl
					overflow="$( perl $checkerFile $nodes $pl $scl | grep Scaled | cut -d ' ' -f 5 )"
					if [[ $overflow == "" ]]; then
						overflow=error
					fi
					status="$testcase has overflow."
					statusList="$statusList $status"
				else
					overflow=error
					wirelength=error
					runtime=error
					status="Please read the README.md."
				fi
			else
				status="$testcase failed."
				statusList="$statusList $status"
			fi

			printf "%15s | %15s | %10s | %s\n" $overflow $wirelength $runtime "$status"
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
