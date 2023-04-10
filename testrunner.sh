#! /bin/bash

for file in 107/*.py
do
    echo $file
    ./puny $file
done
