#!/bin/bash
gcc -o ./bin/main ./src/jpg_dec.c ./src/jpg_dec.h ./src/arraylist.h
./bin/main $1
