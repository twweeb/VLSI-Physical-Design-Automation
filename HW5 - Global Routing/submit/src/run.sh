#!/bin/bash

testnum=${1}
make clean > /dev/null
make > /dev/null

test=../testcase/ibm0$testnum.modified.txt
output=../output/ibm0$testnum.result

rm -f $output

printf "\n------------ RunTest ------------\n"
printf "testcase: $testnum\n"
#echo ../bin/hw5 $test $output
time ../bin/hw5 $test $output

printf "\n------------ Verify -------------\n"
#echo ../verifier/verify $test $output
../verifier/verify $test $output

printf "\n-------------- End --------------\n"