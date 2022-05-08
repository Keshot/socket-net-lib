#!/bin/bash
g++ -I../lib/include/ -g3 -Wall -o test1.out test_main1.cpp ../lib/include/utils/error_handler.cpp ../lib/include/core/core.cpp