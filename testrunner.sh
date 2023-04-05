#! /bin/bash

for file in tests/*/*.py
do
    echo $file
    ./puny $file
done
