#!/bin/bash

# Valgrind tester script for checking memory leaks and errors
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 5, 2021

BUILD_DIR=../build/

(cd ../ && make tests)

for f in ${BUILD_DIR}/*; do
    if [ ${f: -2} != ".o" ] ; then
        
        # Check for errors
        valgrind --leak-check=full --error-exitcode=1 "./$f" >& /dev/null
        RESVAL1=$?

        # Check for leaks
        valgrind --leak-check=full "./$f" 2>&1 | grep "no leaks are possible" > /dev/null
        RESVAL2=$?

        if [[ ${RESVAL1} == 0 ]] && [[ ${RESVAL2} == 0 ]] ; then
            echo "Valgrind test success: $f"
        else
            echo "Valgrind test fail: $f"
        fi
    fi
done