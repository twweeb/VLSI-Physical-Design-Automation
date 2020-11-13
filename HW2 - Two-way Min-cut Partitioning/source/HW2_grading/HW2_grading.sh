#!/bin/bash
homeworkName=HW2
binaryName=hw2
testcasePool="pub2-1 pub2-2 pub2-3 pub2-4 pub2-5 hid2-1 hid2-2 hid2-3"
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
	local argv="$testcaseDir/$1.nets $testcaseDir/$1.cells $outputDir/$1.out"
	local log=$( ( time -p timeout $timeLimit ./$binaryName $argv ) 2>&1 > /dev/null )
	if [[ $log =~ "perl -e" ]]; then
		echo "TLE"
	else
		echo "$( echo "$log" | grep real | cut -d ' ' -f 2 )"
	fi
}

function verifyCmd ()
{
	local argv="$testcaseDir/$1.nets $testcaseDir/$1.cells $outputDir/$1.out"
	local log=$( $verifyBin $argv | cat )
	if [[ $log =~ "open" ]]; then
		echo "$1 run error."
	elif [[ $log =~ "test" ]]; then
		echo "Some cells in $1 are missing. "
	elif [[ $log =~ "cells" ]]; then
		echo "$1 has wrong cell number. "
	elif [[ $log =~ "balance" ]]; then
		echo "$1 is not balance. "
	elif [[ $log =~ "cut size" ]]; then
		echo "$1 has wrong cut size. "
	elif [[ $log =~ "Error" ]]; then
		echo "$1 has something error. "
	else
		echo "success"
	fi
}

echo "|------------------------------------------------|"
echo "|                                                |"
echo "|    This script is used for PDA $homeworkName grading.    |"
echo "|                                                |"
echo "|------------------------------------------------|"

csvTitle="student id"
for testcase in $testcasePool; do
	csvTitle="$csvTitle, $testcase cut size, $testcase runtime"
done
echo "$csvTitle, status" > $csvFile

cd $studentDir/
for studentId in *; do
	if [[ -d $studentId ]]; then 
		printf "grading on %s:\n" $studentId
		cd $studentId

		rm -rf $( find . -mindepth 1 -maxdepth 1 -type d )
		tar -zxvf CS6135_${homeworkName}_$studentId.tar.gz > /dev/null
		find . -name ".*" -delete
		cd $homeworkName/

		cd src
		make clean > /dev/null
		rm -f ./bin/*
		make > /dev/null
		cd ..

		rm -rf $outputDir/*
		cd bin/
		csvContent="$studentId"
		statusList=""
		printf "  testcase |   cut size |    runtime | status\n"
		for testcase in $testcasePool; do
			printf "%10s | " $testcase

			runtime=$( executionCmd $testcase )
			if [[ $runtime != "TLE" ]]; then
				status=$( verifyCmd $testcase )

				if [[ $status == "success" ]]; then
					cutSize=$( cat $outputDir/$testcase.out | grep cut_size | cut -d ' ' -f 2 )
				else
					cutSize=fail
					statusList="$statusList $status"
				fi
			else
				cutSize=fail
				status="$testcase failed."
				statusList="$statusList $status"
			fi

			printf "%10s | %10s | %s\n" $cutSize $runtime "$status"
			csvContent="$csvContent, $cutSize, $runtime"
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
