#!/bin/bash
mkdir "_build"
cd "_build"
cmake .. -DDEV_MODE=True
make && make test
