#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
logfile="test.log"
logfile2="test2.log"
logfile3="test3.log"
dut=$1
dasm=$2

run_test() {
	testcase=$1
    echo "Running test ${testcase##$DIR/} ..."
    $dut $testcase >$logfile
    $dasm <$logfile >$logfile2
    $dut $logfile2 >$logfile3
}


run_all_tests() {
    for t in $DIR/passes/*
    do
    	run_test $t
    	if [ $? -ne 0 ]; then
        	echo "ERROR: Test $t failed. Refer to $logfile"
        	exit 1
    	fi
    done

    diff -q $logfile $logfile3
    if [ $? -ne 0 ]; then
        echo "ERROR: Output of program $t differs"
        exit 1
    fi

    rm -f $logfile $logfile2 $logfile3
}


run_all_tests