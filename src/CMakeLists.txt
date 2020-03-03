project("oberon-lang" VERSION "0.0.0")

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
        symtab/SymbolTable.cpp symtab/SymbolTable.h
        symtab/Scope.cpp symtab/Scope.h
        ast/NodeVisitor.h
        ast/Node.cpp ast/Node.h
        ast/ExpressionNode.cpp ast/ExpressionNode.h
        ast/LiteralNode.cpp ast/LiteralNode.h
        ast/TypeNode.cpp ast/TypeNode.h
        ast/ArrayTypeNode.cpp ast/ArrayTypeNode.h
        ast/BasicTypeNode.cpp ast/BasicTypeNode.h
        ast/RecordTypeNode.cpp ast/RecordTypeNode.h
        ast/BlockNode.cpp ast/BlockNode.h
        ast/ModuleNode.cpp ast/ModuleNode.h
        ast/ProcedureNode.cpp ast/ProcedureNode.h
        ast/DeclarationNode.cpp ast/DeclarationNode.h
        ast/ReferenceNode.cpp ast/ReferenceNode.h
        ast/StatementNode.cpp ast/StatementNode.h
        ast/AssignmentNode.cpp ast/AssignmentNode.h
        ast/CallNode.cpp ast/CallNode.h
        ast/IfThenElseNode.cpp ast/IfThenElseNode.h
        ast/LoopNode.cpp ast/LoopNode.h
        ast/StatementSequenceNode.cpp ast/StatementSequenceNode.h
        ast/NodePrettyPrinter.cpp ast/NodePrettyPrinter.h
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
