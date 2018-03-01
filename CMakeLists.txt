cmake_minimum_required(VERSION 3.9)
project(oberon0c)

set(CMAKE_CXX_STANDARD 14)

add_executable(oberon0c main.cpp
        util/Logger.cpp util/Logger.h
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/NumberToken.cpp scanner/NumberToken.h
        scanner/StringToken.cpp scanner/StringToken.h
        parser/Parser.cpp parser/Parser.h
        parser/SymbolTable.cpp parser/SymbolTable.h
        parser/symbol/Symbol.cpp parser/symbol/Symbol.h
        parser/symbol/ParameterSymbol.cpp parser/symbol/ParameterSymbol.h
        parser/symbol/ProcedureSymbol.cpp parser/symbol/ProcedureSymbol.h
        parser/ast/Node.cpp parser/ast/Node.h
        parser/ast/ExpressionNode.cpp parser/ast/ExpressionNode.h
        parser/ast/UnaryExpressionNode.cpp parser/ast/UnaryExpressionNode.h
        parser/ast/BinaryExpressionNode.cpp parser/ast/BinaryExpressionNode.h
        parser/ast/ConstantNode.cpp parser/ast/ConstantNode.h
        parser/ast/BooleanConstantNode.cpp parser/ast/BooleanConstantNode.h
        parser/ast/NumberConstantNode.cpp parser/ast/NumberConstantNode.h
        parser/ast/StringConstantNode.cpp parser/ast/StringConstantNode.h
        parser/ast/TypeNode.cpp parser/ast/TypeNode.h
        parser/ast/BasicTypeNode.cpp parser/ast/BasicTypeNode.h
        parser/ast/ArrayTypeNode.cpp parser/ast/ArrayTypeNode.h
        parser/ast/RecordTypeNode.cpp parser/ast/RecordTypeNode.h
        parser/ast/TypeReferenceNode.cpp parser/ast/TypeReferenceNode.h
        parser/ast/FieldNode.cpp parser/ast/FieldNode.h parser/ast/ConstantReferenceNode.cpp parser/ast/ConstantReferenceNode.h)
