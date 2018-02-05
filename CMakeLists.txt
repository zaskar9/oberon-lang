cmake_minimum_required(VERSION 3.9)
project(oberon0c)

set(CMAKE_CXX_STANDARD 11)

add_executable(oberon0c main.cpp scanner/Scanner.cpp scanner/Scanner.h ast/ASTNode.cpp ast/ASTNode.h parser/Parser.cpp parser/Parser.h)