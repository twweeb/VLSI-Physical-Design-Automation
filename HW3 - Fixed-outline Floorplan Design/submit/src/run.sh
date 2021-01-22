#!/bin/bash

test=n${1}
ratio=${2}
test_hidden_flag=${2}
EXE='../bin/hw3'
VERIFY='../verifier/verify'
TESTCASE='../testcase/'
OUTPUT='../output/'


rm ${EXE}
make

if [ "$ratio" == "0.1" ]; then
    RES=$test"_1"
else
    RES=$test"_15"
fi

echo 'Test '$test
time ${EXE} ${TESTCASE}${test}.hardblocks ${TESTCASE}${test}.nets ${TESTCASE}${test}.pl ${OUTPUT}${RES}.floorplan ${ratio}
echo 'Verify'
${VERIFY} ${TESTCASE}${test}.hardblocks ${TESTCASE}${test}.nets ${TESTCASE}${test}.pl ${OUTPUT}${RES}.floorplan ${ratio}