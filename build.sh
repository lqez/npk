#!/bin/bash
mkdir "_build"
cd "_build"
if [ "$TRAVIS_OS_NAME" == "windows" ]; then
    cmake .. -DCMAKE_SH="CMAKE_SH-NOTFOUND" -G "MinGW Makefiles" -DDEV_MODE=True
    ls -l
    mingw32-make
else
    cmake .. -DDEV_MODE=True
    make && make test
fi
