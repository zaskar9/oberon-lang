/*
 * Semantic analysis pass used by the analyzer of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#ifndef OBERON_LLVM_SEMANTICANALYSIS_H
#define OBERON_LLVM_SEMANTICANALYSIS_H


#include "Analyzer.h"
#include "data/ast/NodeVisitor.h"
#include "data/symtab/SymbolTable.h"
#include "data/symtab/SymbolImporter.h"
#include "data/symtab/SymbolExporter.h"
#include <map>

class SemanticAnalysis final : public Analysis, private NodeVisitor {

private:
    [[maybe_unused]] CompilerFlags *flags_;
    SymbolTable *symbols_;
    Logger *logger_;
    ModuleNode *module_;
    BlockNode *parent_;
    SymbolImporter *importer_;
    SymbolExporter *exporter_;
    std::map<std::string, PointerTypeNode *> forwards_;
    TypeNode *tBoolean_, *tByte_, *tChar_, *tInteger_, *tReal_, *tLongReal_, *tString_;

    void block(BlockNode &node);
    void call(ProcedureNodeReference &node);

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(ImportNode &node) override;

    void visit(ConstantDeclarationNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(TypeDeclarationNode &node) override;
    void visit(VariableDeclarationNode &node) override;

    void visit(TypeReferenceNode &node) override;
    void visit(ValueReferenceNode &node) override;

    void visit(BooleanLiteralNode &node) override;
    void visit(IntegerLiteralNode &node) override;
    void visit(RealLiteralNode &node) override;
    void visit(StringLiteralNode &node) override;
    void visit(NilLiteralNode &node) override;

    void visit(UnaryExpressionNode &node) override;
    void visit(BinaryExpressionNode &node) override;

    void visit(ArrayTypeNode &node) override;
    void visit(BasicTypeNode &node) override;
    void visit(ProcedureTypeNode &node) override;
    void visit(RecordTypeNode &node) override;
    void visit(PointerTypeNode &node) override;

    void visit(StatementSequenceNode &node) override;
    void visit(AssignmentNode &node) override;
    void visit(IfThenElseNode &node) override;
    void visit(ElseIfNode &node) override;
    void visit(ProcedureCallNode &node) override;
    void visit(LoopNode &node) override;
    void visit(WhileLoopNode &node) override;
    void visit(RepeatLoopNode &node) override;
    void visit(ForLoopNode &node) override;
    void visit(ReturnNode &node) override;

    bool assertEqual(Ident *aIdent, Ident *bIdent) const;
    void assertUnique(Ident *ident, Node &node);
    bool assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual, bool isPtr = false);
    void checkExport(DeclarationNode &node);

    std::unique_ptr<LiteralNode> fold(const ExpressionNode *expr) const;
    long foldInteger(const ExpressionNode *expr) const;
    double foldReal(const ExpressionNode *expr) const;
    bool foldBoolean(const ExpressionNode *expr) const;
    std::string foldString(const ExpressionNode *expr) const;

    std::string format(const TypeNode *type, bool isPtr = false) const;

    TypeNode *commonType(TypeNode *lhsType, TypeNode *rhsType) const;
    TypeNode *resolveType(TypeNode *type);

public:
    explicit SemanticAnalysis(CompilerFlags *flags, SymbolTable *symbols, SymbolImporter *importer, SymbolExporter *exporter);
    ~SemanticAnalysis() override = default;

    void run(Logger *logger, Node *node) override;

};


#endif //OBERON_LLVM_SEMANTICANALYSIS_H
