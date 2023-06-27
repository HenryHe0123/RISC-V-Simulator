cmake_minimum_required(VERSION 3.16)
project(RISC_V_simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(code
        main.cpp
        lib/register.h
        lib/memory.h
        cpu.h
        lib/decoder.h
        lib/ALU.h
        lib/ReservedStation.h
        lib/ReorderBuffer.h
        lib/queue.h
        lib/InstructionUnit.h
        lib/predictor.h)
