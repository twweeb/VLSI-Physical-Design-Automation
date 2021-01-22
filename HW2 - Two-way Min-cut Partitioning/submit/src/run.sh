#!/bin/bash

test=2-${1}
test_hidden_flag=${2}
EXE='../bin/hw2'
VERIFY='../verifier/verify'
TESTCASE='../testcases/'
OUTPUT='../output/'

rm ${EXE}
make

if [ "$test_hidden_flag" == "-h" ]; then
    echo 'Test h'$test
    time ${EXE} ${TESTCASE}h${test}.nets ${TESTCASE}h${test}.cells ${OUTPUT}h${test}.out
    echo 'Verify'
    ${VERIFY} ${TESTCASE}h${test}.nets ${TESTCASE}h${test}.cells ${OUTPUT}h${test}.out
else
    echo 'Test p'$test
    time ${EXE} ${TESTCASE}p${test}.nets ${TESTCASE}p${test}.cells ${OUTPUT}p${test}.out
    echo 'Verify'
    ${VERIFY} ${TESTCASE}p${test}.nets ${TESTCASE}p${test}.cells ${OUTPUT}p${test}.out
fi
