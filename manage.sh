#!/bin/bash

# -------------------------------------------------------------------------------------------------
# Management script
# -------------------------------------------------------------------------------------------------

if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit
fi


# -------------------------------------------------------------------------------------------------
# Building
# -------------------------------------------------------------------------------------------------

function build {
    mkdir -p docs/images &&
    ln -sf $(pwd)/data/screenshots $(pwd)/docs/images/ &&
    mkdir -p build &&
    cd build && \
    cmake .. -GNinja && \
    ninja && \
    cd ..
}

function rmbuild {
    rm -rf build
}


# -------------------------------------------------------------------------------------------------
# Cython
# -------------------------------------------------------------------------------------------------

function build_cython {
    rm datoviz/*.c && \
    python3 tools/generate_cython.py && \
    python3 setup.py build_ext -i && \
    python3 setup.py develop --user
}



# -------------------------------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------------------------------

function test {
    ./build/datoviz test $1
}



# -------------------------------------------------------------------------------------------------
# Entry-point
# -------------------------------------------------------------------------------------------------

if [ $1 == "build" ]
then
    build
elif [ $1 == "rebuild" ]
then
    rmbuild
    build
elif [ $1 == "cython" ]
then
    build_cython
elif [ $1 == "test" ]
then
    test $2
fi
