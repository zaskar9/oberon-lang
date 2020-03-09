project("oberon-lang" VERSION "0.0.0")

set(Boost_USE_MULTITHREADED TRUE)

set(SOURCE_FILES
        main.cpp
        util/Logger.cpp util/Logger.h
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/LiteralToken.cpp scanner/LiteralToken.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h
        data/ast/NodeVisitor.h
        data/ast/Node.cpp data/ast/Node.h
        data/ast/ExpressionNode.cpp data/ast/ExpressionNode.h
        data/ast/TypeNode.cpp data/ast/TypeNode.h
        data/ast/ArrayTypeNode.cpp data/ast/ArrayTypeNode.h
        data/ast/BasicTypeNode.cpp data/ast/BasicTypeNode.h
        data/ast/RecordTypeNode.cpp data/ast/RecordTypeNode.h
        data/ast/BlockNode.cpp data/ast/BlockNode.h
        data/ast/ModuleNode.cpp data/ast/ModuleNode.h
        data/ast/ProcedureNode.cpp data/ast/ProcedureNode.h
        data/ast/DeclarationNode.cpp data/ast/DeclarationNode.h
        data/ast/NodeReference.cpp data/ast/NodeReference.h
        data/ast/StatementNode.cpp data/ast/StatementNode.h
        data/ast/AssignmentNode.cpp data/ast/AssignmentNode.h
        data/ast/IfThenElseNode.cpp data/ast/IfThenElseNode.h
        data/ast/LoopNode.cpp data/ast/LoopNode.h
        data/ast/StatementSequenceNode.cpp data/ast/StatementSequenceNode.h
        data/ast/NodePrettyPrinter.cpp data/ast/NodePrettyPrinter.h
        data/symtab/SymbolTable.cpp data/symtab/SymbolTable.h
        data/symtab/Scope.cpp data/symtab/Scope.h
        parser/Parser.cpp parser/Parser.h
        analyzer/Analyzer.cpp analyzer/Analyzer.h
        analyzer/SemanticAnalysis.cpp analyzer/SemanticAnalysis.h
        llvm/LLVMIRBuilder.cpp llvm/LLVMIRBuilder.h
        llvm/LLVMCompiler.cpp llvm/LLVMCompiler.h analyzer/LambdaLifter.cpp analyzer/LambdaLifter.h)

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
