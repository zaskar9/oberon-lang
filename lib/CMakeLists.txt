set(OLANG_BULK "olang-bulk" CACHE INTERNAL "Target for main library.")
set(OLANG_BULK_NAME "OlangBulk")

add_library(global INTERFACE)
target_include_directories(global INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(logger)
add_subdirectory(scanner)
add_subdirectory(linker)

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
        data/ast/CaseOfNode.cpp data/ast/CaseOfNode.h
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
        ${DATA_SOURCES}
        ${SYMTAB_SOURCES}
        ${PARSER_SOURCES}
        ${SEMA_SOURCES}
        ${SYSTEM_SOURCES}
        ${ANALYZER_SOURCES}
        ${CODEGEN_SOURCES}
        ${COMPILER_SOURCES})

if (BUILD_SHARED_LIBS)
    add_library(${OLANG_BULK} SHARED ${ALL_SOURCES})
else ()
    add_library(${OLANG_BULK} STATIC ${ALL_SOURCES})
endif ()

set_target_properties(${OLANG_BULK} PROPERTIES OUTPUT_NAME ${OLANG_BULK_NAME})

target_include_directories(${OLANG_BULK} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

# Include internal dependencies
target_link_libraries(${OLANG_BULK} PUBLIC global)
target_link_libraries(${OLANG_BULK} PRIVATE ${OLANG_LEX} ${OLANG_LOG})

# Include external dependencies
if (Boost_FOUND)
    target_include_directories(${OLANG_BULK} SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    target_link_libraries(${OLANG_BULK} PRIVATE Boost::headers)
endif ()
if (LLVM_FOUND)
    target_include_directories(${OLANG_BULK} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${OLANG_BULK} PRIVATE ${llvm_libs})
endif ()
#if (BDWgc_FOUND)
#    target_link_libraries(${OBERON_LANG} PRIVATE BDWgc::gc)
#endif ()