cmake_minimum_required(VERSION 3.15)

project(oberon-lang VERSION "0.0.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(Boost_USE_MULTITHREADED TRUE)

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
        scanner/LiteralToken.cpp scanner/LiteralToken.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h
        parser/Parser.cpp parser/Parser.h
        data/symtab/SymbolTable.cpp data/symtab/SymbolTable.h
        data/symtab/Scope.cpp data/symtab/Scope.h
        data/ast/NodeVisitor.h
        data/ast/Node.cpp data/ast/Node.h
        data/ast/ExpressionNode.cpp data/ast/ExpressionNode.h
        data/ast/LiteralNode.cpp data/ast/LiteralNode.h
        data/ast/TypeNode.cpp data/ast/TypeNode.h
        data/ast/ArrayTypeNode.cpp data/ast/ArrayTypeNode.h
        data/ast/BasicTypeNode.cpp data/ast/BasicTypeNode.h
        data/ast/RecordTypeNode.cpp data/ast/RecordTypeNode.h
        data/ast/BlockNode.cpp data/ast/BlockNode.h
        data/ast/ModuleNode.cpp data/ast/ModuleNode.h
        data/ast/ProcedureNode.cpp data/ast/ProcedureNode.h
        data/ast/DeclarationNode.cpp data/ast/DeclarationNode.h
        data/ast/ReferenceNode.cpp data/ast/ReferenceNode.h
        data/ast/StatementNode.cpp data/ast/StatementNode.h
        data/ast/AssignmentNode.cpp data/ast/AssignmentNode.h
        data/ast/CallNode.cpp data/ast/CallNode.h
        data/ast/IfThenElseNode.cpp data/ast/IfThenElseNode.h
        data/ast/LoopNode.cpp data/ast/LoopNode.h
        data/ast/StatementSequenceNode.cpp data/ast/StatementSequenceNode.h
        data/ast/NodePrettyPrinter.cpp data/ast/NodePrettyPrinter.h
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
