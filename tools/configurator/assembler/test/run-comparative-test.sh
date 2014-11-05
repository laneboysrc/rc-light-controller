#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
dut=$1
logfile="test.log"
dut2=$2
logfile2="test2.log"


run_test() {
	testcase=$1
    echo "Running test ${testcase##$DIR/} ..."
    echo "$testcase" > $logfile
    echo "" >> $logfile
    $dut $testcase >>$logfile 2>/dev/null
}

run_test2() {
    testcase=$1
    #echo "Running test ${testcase##$DIR/} ..."
    echo "$testcase" > $logfile2
    echo "" >> $logfile2
    $dut2 $testcase >>$logfile2 2>/dev/null
}

run_all_tests() {
    for t in $DIR/passes/*
    do
    	run_test $t
    	if [ $? -ne 0 ]; then
        	echo "ERROR: Test $t failed. Refer to $logfile"
        	exit 1
    	fi

        run_test2 $t
        if [ $? -ne 0 ]; then
            echo "ERROR: Test2 $t failed. Refer to $logfile2"
            exit 1
        fi

        diff -q $logfile $logfile2
        if [ $? -ne 0 ]; then
            echo "ERROR: Output of program $t differs"
            exit 1
        fi
    done

    rm $logfile $logfile2
}


run_all_tests