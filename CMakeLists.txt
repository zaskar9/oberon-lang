cmake_minimum_required(VERSION 3.15)

project(oberon-lang VERSION "0.0.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-W -Wall -Wextra -Werror -pedantic -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default)
endif()

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
        llvm/LLVMIRBuilder.cpp llvm/LLVMIRBuilder.h
        llvm/LLVMCompiler.cpp llvm/LLVMCompiler.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(llvm_libs core support passes ${LLVM_TARGETS_TO_BUILD})
    target_link_libraries(${PROJECT_NAME} ${llvm_libs})
endif()

configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
