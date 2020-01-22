cmake_minimum_required(VERSION 3.9)
project(oberon0c)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default")

set(SOURCE_FILES
        main.cpp
        util/Logger.cpp util/Logger.h
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/NumberToken.cpp scanner/NumberToken.h
        scanner/StringToken.cpp scanner/StringToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h
        parser/Parser.cpp parser/Parser.h
        parser/symbol/SymbolTable.cpp parser/symbol/SymbolTable.h
        parser/symbol/Scope.cpp parser/symbol/Scope.h
        parser/ast/NodeVisitor.h
        parser/ast/Node.cpp parser/ast/Node.h
        parser/ast/ExpressionNode.cpp parser/ast/ExpressionNode.h
        parser/ast/UnaryExpressionNode.cpp parser/ast/UnaryExpressionNode.h
        parser/ast/BinaryExpressionNode.cpp parser/ast/BinaryExpressionNode.h
        parser/ast/LiteralNode.cpp parser/ast/LiteralNode.h
        parser/ast/TypeNode.cpp parser/ast/TypeNode.h
        parser/ast/BasicTypeNode.cpp parser/ast/BasicTypeNode.h
        parser/ast/ArrayTypeNode.cpp parser/ast/ArrayTypeNode.h
        parser/ast/RecordTypeNode.cpp parser/ast/RecordTypeNode.h
        parser/ast/BlockNode.cpp parser/ast/BlockNode.h
        parser/ast/ModuleNode.cpp parser/ast/ModuleNode.h
        parser/ast/ProcedureNode.cpp parser/ast/ProcedureNode.h
        parser/ast/DeclarationNode.cpp parser/ast/DeclarationNode.h
        parser/ast/ReferenceNode.cpp parser/ast/ReferenceNode.h
        parser/ast/StatementNode.cpp parser/ast/StatementNode.h
        parser/ast/AssignmentNode.cpp parser/ast/AssignmentNode.h
        parser/ast/ProcedureCallNode.cpp parser/ast/ProcedureCallNode.h
        parser/ast/IfThenElseNode.cpp parser/ast/IfThenElseNode.h
        parser/ast/LoopNode.cpp parser/ast/LoopNode.h
        parser/ast/StatementSequenceNode.cpp parser/ast/StatementSequenceNode.h
        parser/ast/NodePrettyPrinter.cpp parser/ast/NodePrettyPrinter.h
        codegen/BasicBlock.cpp codegen/BasicBlock.h
        codegen/Instruction.cpp codegen/Instruction.h
        codegen/Operand.cpp codegen/Operand.h
        codegen/CodeGenerator.cpp codegen/CodeGenerator.h
        codegen/NasmAssembly.cpp codegen/NasmAssembly.h
        codegen/Section.cpp codegen/Section.h)

set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multi-threading

find_package(Boost COMPONENTS system filesystem REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(oberon0c ${SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()