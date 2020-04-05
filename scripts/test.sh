#!/usr/bin/env bash

CALC_DEV=/dev/calc
CALC_MOD=calc.ko
LIVEPATCH_CALC_MOD=livepatch-calc.ko

source scripts/eval.sh

test_op() {
    local expression=$1 
    echo "Testing " ${expression} "..."
    echo -ne ${expression}'\0' > $CALC_DEV
    fromfixed $(cat $CALC_DEV)
}

if [ "$EUID" -eq 0 ]
  then echo "Don't run this script as root"
  exit
fi

sudo rmmod -f livepatch-calc 2>/dev/null
sudo rmmod -f calc 2>/dev/null
sleep 1

modinfo $CALC_MOD || exit 1
sudo insmod $CALC_MOD
sudo chmod 0666 $CALC_DEV
echo



# Livepatch
sudo insmod $LIVEPATCH_CALC_MOD
sleep 1
echo "livepatch was applied"

# Calculate exe. time

ts=$(date +%s%N)
test_op 'fib(10)'
tt=$(($(date +%s%N) - $ts))


echo "Time taken: $tt nanoseconds"

dmesg | tail -n 6
echo "Disabling livepatch..."
sudo sh -c "echo 0 > /sys/kernel/livepatch/livepatch_calc/enabled"
sleep 2
sudo rmmod livepatch-calc

sudo rmmod calc

# epilogue
echo "Complete"
