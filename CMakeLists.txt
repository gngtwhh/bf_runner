cmake_minimum_required(VERSION 3.28)
project(bf_runner C)

set(CMAKE_C_STANDARD 11)

add_executable(bf_runner
        main.c
        bf.c
        bf.h
        command.c
        command.h
        debug_sig.h)
