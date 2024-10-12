cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Compiler main.cpp
        Lexer.cpp
        FileIO.h
        FileIO.cpp
        Node.h
        Parser.cpp
        Parser.h
)
