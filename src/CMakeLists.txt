set(Boost_USE_MULTITHREADED TRUE)
set(OBERON_LANG "oberon-lang")
set(GRAMMAR_TOOL "grammar-tool")

set(Boost_NO_WARN_NEW_VERSIONS 1)

# add source tree to search path to avoid relative includes
include_directories(BEFORE .)

set(LOGGING_SOURCES
        logging/Logger.cpp logging/Logger.h)

set(SCANNER_SOURCES
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/LiteralToken.cpp scanner/LiteralToken.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h)

set(DATA_SOURCES
        data/ast/ASTContext.cpp data/ast/ASTContext.h
        data/ast/Ident.cpp data/ast/Ident.h
        data/ast/Designator.cpp data/ast/Designator.h
        data/ast/Node.cpp data/ast/Node.h
        data/ast/ExpressionNode.cpp data/ast/ExpressionNode.h
        data/ast/TypeNode.cpp data/ast/TypeNode.h
        data/ast/ArrayTypeNode.cpp data/ast/ArrayTypeNode.h
        data/ast/BasicTypeNode.cpp data/ast/BasicTypeNode.h
        data/ast/PointerTypeNode.cpp data/ast/PointerTypeNode.h
        data/ast/ProcedureTypeNode.cpp data/ast/ProcedureTypeNode.h
        data/ast/RecordTypeNode.cpp data/ast/RecordTypeNode.h
        data/ast/BlockNode.cpp data/ast/BlockNode.h
        data/ast/ModuleNode.cpp data/ast/ModuleNode.h
        data/ast/ImportNode.cpp data/ast/ImportNode.h
        data/ast/ProcedureNode.cpp data/ast/ProcedureNode.h
        data/ast/DeclarationNode.cpp data/ast/DeclarationNode.h
        data/ast/StatementNode.cpp data/ast/StatementNode.h
        data/ast/AssignmentNode.cpp data/ast/AssignmentNode.h
        data/ast/IfThenElseNode.cpp data/ast/IfThenElseNode.h
        data/ast/LoopNode.cpp data/ast/LoopNode.h
        data/ast/StatementSequenceNode.cpp data/ast/StatementSequenceNode.h
        data/ast/NodeReference.cpp data/ast/NodeReference.h
        data/ast/NodeVisitor.cpp data/ast/NodeVisitor.h
        data/ast/NodePrettyPrinter.cpp data/ast/NodePrettyPrinter.h)

set(SYMTAB_SOURCES
        data/symtab/SymbolTable.cpp data/symtab/SymbolTable.h
        data/symtab/Scope.cpp data/symtab/Scope.h
        data/symtab/SymbolFile.cpp data/symtab/SymbolFile.h
        data/symtab/SymbolExporter.cpp data/symtab/SymbolExporter.h
        data/symtab/SymbolImporter.cpp data/symtab/SymbolImporter.h)

set(PARSER_SOURCES
        parser/Parser.cpp parser/Parser.h)

set(SEMA_SOURCES
        sema/Sema.cpp sema/Sema.h)

set(SYSTEM_SOURCES
        system/OberonSystem.cpp system/OberonSystem.h
        system/PredefinedProcedure.cpp system/PredefinedProcedure.h)

set(ANALYZER_SOURCES
        analyzer/Analyzer.cpp analyzer/Analyzer.h
        analyzer/LambdaLifter.cpp analyzer/LambdaLifter.h)

set(CODEGEN_SOURCES
        codegen/CodeGen.cpp codegen/CodeGen.h
        codegen/llvm/LLVMIRBuilder.cpp codegen/llvm/LLVMIRBuilder.h
        codegen/llvm/LLVMCodeGen.cpp codegen/llvm/LLVMCodeGen.h)

set(COMPILER_SOURCES
        compiler/CompilerConfig.cpp compiler/CompilerConfig.h
        compiler/CompilationStatus.cpp compiler/CompilationStatus.h
        compiler/Compiler.cpp compiler/Compiler.h)

set(ALL_SOURCES
        global.h
        ${LOGGING_SOURCES}
        ${SCANNER_SOURCES}
        ${DATA_SOURCES}
        ${SYMTAB_SOURCES}
        ${PARSER_SOURCES}
        ${SEMA_SOURCES}
        ${SYSTEM_SOURCES}
        ${ANALYZER_SOURCES}
        ${CODEGEN_SOURCES}
        ${COMPILER_SOURCES})

add_executable(${OBERON_LANG} main.cpp ${ALL_SOURCES})
install (TARGETS ${OBERON_LANG} RUNTIME DESTINATION bin)

target_include_directories(${OBERON_LANG} PUBLIC .)
# main executable needs to export symbols (of statically linked LLVM) for JIT mode
set_target_properties(${OBERON_LANG} PROPERTIES ENABLE_EXPORTS ON)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)
if (Boost_FOUND)
    target_link_libraries(${OBERON_LANG} PRIVATE Boost::boost Boost::program_options)
endif ()

include(GNUInstallDirs)

# disabled options below somehow interfere with other targets
find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
    if (${LLVM_VERSION} LESS 16.0.0)
        message(STATUS "Warning: found legacy LLVM: ${LLVM_VERSION}")
        add_definitions(-D_LLVM_LEGACY)
    else ()
        message(STATUS "Found LLVM: ${LLVM_VERSION}")
        if (${LLVM_VERSION} MATCHES "16.*.*")
            add_definitions(-D_LLVM_16)
        elseif (${LLVM_VERSION} MATCHES "17.*.*")
            add_definitions(-D_LLVM_17)
        elseif (${LLVM_VERSION} MATCHES "18.*.*")
            add_definitions(-D_LLVM_18)
        endif()
    endif ()
    # list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
    # include(HandleLLVMOptions)
    add_definitions(${LLVM_DEFINITIONS})
    # target_include_directories(${OBERON_LANG} PRIVATE ${LLVM_INCLUDE_DIRS})
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    llvm_map_components_to_libnames(llvm_libs core executionengine jitlink orcjit nativecodegen support passes ${LLVM_TARGETS_TO_BUILD})
    target_link_libraries(${OBERON_LANG} PRIVATE ${llvm_libs})
endif ()


set(GRAMMAR_SOURCES
        compiler/CompilerConfig.cpp compiler/CompilerConfig.h
        grammar/Grammar.cpp grammar/Grammar.h
        grammar/Validator.cpp grammar/Validator.h
        grammar/Loader.cpp grammar/Loader.h)

add_executable(${GRAMMAR_TOOL} grammar/main.cpp ${GRAMMAR_SOURCES} ${SCANNER_SOURCES} ${LOGGING_SOURCES})


configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
