#!/bin/bash

clear

echo "Script for BAD"
echo
echo "Compiling the code"
echo
make all
echo
./admittance
echo
python plotadmittance.py
