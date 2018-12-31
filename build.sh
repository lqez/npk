#!/bin/bash
if [ "$TRAVIS_OS_NAME" == "windows" ]; then
    mkdir "_build"
    cd "_build"
    cmake .. -DDEV_MODE=True
    nmake && nmake test
else
    mkdir "_build"
    cd "_build"
    cmake .. -DDEV_MODE=True
    make && make test
fi
