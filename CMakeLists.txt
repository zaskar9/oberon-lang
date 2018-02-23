cmake_minimum_required(VERSION 3.9)
project(oberon0c)

set(CMAKE_CXX_STANDARD 14)

add_executable(oberon0c main.cpp
        util/Logger.cpp util/Logger.h
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/NumberToken.cpp scanner/NumberToken.h
        parser/Parser.cpp parser/Parser.h
        parser/ast/ASTNode.cpp parser/ast/ASTNode.h
        parser/ast/ExpressionNode.cpp parser/ast/ExpressionNode.h
        parser/ast/BinaryExpressionNode.cpp parser/ast/BinaryExpressionNode.h
        parser/ast/UnaryExpressionNode.cpp parser/ast/UnaryExpressionNode.h
        parser/ast/ConstantNode.cpp parser/ast/ConstantNode.h
        parser/ast/BooleanConstantNode.cpp parser/ast/BooleanConstantNode.h
        parser/ast/NumberConstantNode.cpp parser/ast/NumberConstantNode.h
        parser/ast/StringConstantNode.cpp parser/ast/StringConstantNode.h
        parser/symbol/Table.cpp parser/symbol/Table.h
        parser/symbol/Symbol.cpp parser/symbol/Symbol.h
        parser/symbol/ParameterSymbol.cpp parser/symbol/ParameterSymbol.h
        parser/symbol/TypeSymbol.cpp parser/symbol/TypeSymbol.h
        parser/symbol/BasicTypeSymbol.cpp parser/symbol/BasicTypeSymbol.h
        parser/symbol/ProcedureSymbol.cpp parser/symbol/ProcedureSymbol.h
        parser/symbol/ArrayTypeSymbol.cpp parser/symbol/ArrayTypeSymbol.h
        parser/symbol/RecordTypeSymbol.cpp parser/symbol/RecordTypeSymbol.h
        parser/symbol/VariableSymbol.cpp parser/symbol/VariableSymbol.h
        parser/symbol/ConstantSymbol.cpp parser/symbol/ConstantSymbol.h
        parser/symbol/NumberConstantSymbol.cpp parser/symbol/NumberConstantSymbol.h
        parser/symbol/BooleanConstantSymbol.cpp parser/symbol/BooleanConstantSymbol.h
        parser/symbol/StringConstantSymbol.cpp parser/symbol/StringConstantSymbol.h)