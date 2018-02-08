cmake_minimum_required(VERSION 3.9)
project(oberon0c)

set(CMAKE_CXX_STANDARD 11)

add_executable(oberon0c main.cpp util/Logger.cpp util/Logger.h scanner/Scanner.cpp scanner/Scanner.h
        parser/Parser.cpp parser/Parser.h parser/ast/ASTNode.cpp parser/ast/ASTNode.h
        parser/symbol/Table.cpp parser/symbol/Table.h parser/symbol/Symbol.cpp parser/symbol/Symbol.h
        parser/symbol/ParameterSymbol.cpp parser/symbol/ParameterSymbol.h parser/symbol/TypeSymbol.cpp parser/symbol/TypeSymbol.h)