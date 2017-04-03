#!/usr/bin/env bash

echo "--------------"
sudo insmod fourmb.ko
echo abc > /dev/fourmb
cat /dev/fourmb
echo defg > /dev/fourmb
cat /dev/fourmb
sudo rmmod fourmb

echo "--------------"
sudo insmod fourmb.ko
time ./test1
head /dev/fourmb
echo "***"
tail /dev/fourmb
sudo rmmod fourmb

echo "--------------"
sudo insmod fourmb.ko
./llseek_test
cat /dev/fourmb
sudo rmmod fourmb
