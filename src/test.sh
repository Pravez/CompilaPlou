#!/bin/bash

logfile='/tmp/log_compil'

echo -ne "Running make... "
if make >> $logfile 2>&1; then
    echo "done"
else
    echo "error !"
    cat $logfile
    rm $logfile
    exit
fi
echo "Retriving tests..."
test_ok=$(find test-c/test-ok/ -name '*.c')

for test in $test_ok; do
    echo -ne "Running "$test"..."
    echo "" > $logfile
    if ./parse $test >> $logfile 2>&1; then
        echo "" > $logfile
        if lli output.ll >>$logfile 2>&1; then
            echo "done."
        else
            echo "error !"
            cat $logfile
            rm $logfile
            exit
        fi
    else
        echo "compilation error !"
        cat $logfile
        rm $logfile
        exit
    fi
done

