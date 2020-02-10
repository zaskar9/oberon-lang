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
        parser/ast/CallNode.cpp parser/ast/CallNode.h
        parser/ast/IfThenElseNode.cpp parser/ast/IfThenElseNode.h
        parser/ast/LoopNode.cpp parser/ast/LoopNode.h
        parser/ast/StatementSequenceNode.cpp parser/ast/StatementSequenceNode.h
        parser/ast/NodePrettyPrinter.cpp parser/ast/NodePrettyPrinter.h
        codegen/nasm/Assembly.cpp codegen/nasm/Assembly.h
        codegen/nasm/Instruction.cpp codegen/nasm/Instruction.h
        codegen/nasm/Formatter.cpp codegen/nasm/Formatter.h
        codegen/nasm/Operand.cpp codegen/nasm/Operand.h
        codegen/nasm/NASMCodeGen.cpp codegen/nasm/NASMCodeGen.h
        codegen/llvm/LLVMCodeGen.cpp codegen/llvm/LLVMCodeGen.h)

add_executable(oberon0c ${SOURCE_FILES})

find_package(Boost COMPONENTS system filesystem REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(llvm_libs all)
    target_link_libraries(oberon0c ${llvm_libs})
endif()

