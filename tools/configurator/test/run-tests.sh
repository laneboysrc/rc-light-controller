#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
logfile="test.log"
dut=$1


run_test() {
	testcase=$1
    echo "Running test ${testcase##$DIR/} ..."
    echo "$dut <$testcase" > $logfile
    echo "" >> $logfile
    $dut <$testcase>>$logfile 2>&1
}


run_all_tests() {
    for t in $DIR/fails/*
    do
    	run_test $t
    	if [ $? -ne 1 ]; then
        	echo "ERROR: Test passed though it should have failed. Refer to $logfile"
        	exit 1
    	fi
    done

    for t in $DIR/passes/*
    do
    	run_test $t
    	if [ $? -ne 0 ]; then
        	echo "ERROR: Test failed. Refer to $logfile"
        	exit 1
    	fi
    done

    rm -f $logfile
}


run_all_tests