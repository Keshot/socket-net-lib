#!/bin/bash
g++ -I../lib/include/ -g3 -Wall -o test.out test_main.cpp ../lib/include/utils/error_handler.cpp ../lib/include/core/core.cpp