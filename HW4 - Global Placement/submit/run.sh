#!/bin/bash
in=${1}

testdict () {
    case "$1" in
        testcases/ibm01 )  test_name="ibm01-cu85"
            ;;
        testcases/ibm02 )  test_name="ibm02-cu90"
            ;;
        testcases/ibm05 )  test_name="ibm05"
            ;;
        testcases/ibm07 )  test_name="ibm07-cu90"
            ;;
        testcases/ibm08 )  test_name="ibm08-cu90"
            ;;
        testcases/ibm09 )  test_name="ibm09-cu90"
            ;;
        *) test_name=$1
    esac
    echo "$test_name"
}

make clean
make

if [ -z "$in" ]
then
    log_file="logs/"$(date +"%s")
    i=0
    mkdir -p "$log_file"
    for test in "testcases/"*
    do
        ./hw4 -aux $test/$(testdict "$test").aux > $log_file/$i.log
        ((i=i+1))
    done
else
    ./hw4 -aux testcases/$in/$(testdict testcases/"$in").aux > $in-$(date +"%s").log
fi
