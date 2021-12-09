#! /usr/bin/env bash

# Setup
PASS_DIR=$PROJ/passes/$1
DRIVER_FILE=$2
TC=$PROJ/passes/toolchain
cd ${TC} && ./clean.sh && cd $PROJ ;
mkdir -p ${TC}/catpass/include ;
mkdir -p ${TC}/catpass/src ;


# Copy the sources into the toolchain for build
cp -a ${PASS_DIR}/* ${TC}/catpass/ ;
# cp -a ${PASS_DIR}/${DRIVER_FILE} ${TC}/catpass/ ;
# cp -a ${PASS_DIR}/include/ ${TC}/catpass/ ;
# cp -a ${PASS_DIR}/src/ ${TC}/catpass/ ;
# cp -a ${PASS_DIR}/CMakeLists.txt ${TC}/catpass/ ;


# Build from the toolchain
cd ${TC} && ./run_me.sh


# Done
cd $PROJ 
