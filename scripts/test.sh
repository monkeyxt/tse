#!/bin/bash

# Unit test batch run script file for list.c
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 5, 2021

BUILD_DIR=../build

(cd ../ && make tests)

for f in ${BUILD_DIR}/*; do
    if [ ${f: -2} != ".o" ]; then
        bash runtest.sh "./$f"
    fi
done 