#!/bin/bash
g++ -I../lib/include/ -g3 -Wall -o client.out client_main.cpp ../lib/include/utils/error_handler.cpp ../lib/include/core/net_socket.cpp