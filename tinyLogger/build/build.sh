#!/bin/bash

cmake ../src/ && make

echo " [*] cleanning make files..."

rm -r ./CMake*
rm -r cmake*
rm Makefile

echo " [✔] build complete"
