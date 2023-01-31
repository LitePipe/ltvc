#!/bin/sh

echo "Generating corpus directories"
python ./vectors2corpus.py

echo "Building fuzz tests"
make clean && make

echo "Running libfuzz Fuzzing"
./fuzz -max_len=5242880 corpus