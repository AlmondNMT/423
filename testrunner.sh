#!/bin/bash

# Before running this script, you may need to convert the line endings
# to Unix format using the 'dos2unix' command:
#
#   dos2unix testrunner.sh
#
# Then, you can run the script using the following command:
#
#   ./testrunner.sh

for file in 107/*.py
do
    echo $file
    ./puny $file
done
