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

echo -e "\n--------------"
sudo insmod fourmb.ko
./test2
sudo rmmod fourmb

echo "--------------"
sudo insmod fourmb.ko
./test3
dmesg | tail
sudo rmmod fourmb
