cmake_minimum_required(VERSION 3.15)
project(VoleTeam)

set(CMAKE_CXX_STANDARD 17)

add_executable(VoleTeam app.cpp Menu.cpp Cpu.cpp Instruction.cpp VoleMachine.cpp ByteStorge.cpp Screen.cpp)