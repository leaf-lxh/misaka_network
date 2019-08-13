#!/bin/bash

config_path=/etc/tinyLogger/
log_path=/var/log/tinyLogger/

echo " [*] installing binary to /usr/bin/"
cp ./tinyLogger /usr/bin/tinyLogger

echo " [*] copy configuration file to" $config_path
mkdir $config_path
cp -r ../src/conf/* $config_path

echo " [*] create log directory" $log_path
mkdir $log_path
