/*
 * Semantic analysis pass used by the analyzer of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#ifndef OBERON_LLVM_SEMANTICANALYSIS_H
#define OBERON_LLVM_SEMANTICANALYSIS_H


#include "Analyzer.h"
#include "../data/ast/NodeVisitor.h"
#include "../data/symtab/SymbolTable.h"

class SemanticAnalysis final : public Analysis, private NodeVisitor {

private:
    SymbolTable *symtab_;
    Logger *logger_;
    BlockNode *parent_;
    TypeNode *tBoolean, *tByte, *tChar, *tInteger, *tReal, *tString;

    void block(BlockNode &node);
    void call(ProcedureNodeReference &node);

    void visit(ModuleNode &node) override;
    void visit(ProcedureNode &node) override;

    void visit(ImportNode &node) override;

    void visit(ConstantDeclarationNode &node) override;
    void visit(FieldNode &node) override;
    void visit(ParameterNode &node) override;
    void visit(VariableDeclarationNode &node) override;

    void visit(TypeReferenceNode &node) override;
    void visit(ValueReferenceNode &node) override;

    void visit(BooleanLiteralNode &node) override;
    void visit(IntegerLiteralNode &node) override;
    void visit(StringLiteralNode &node) override;
    void visit(FunctionCallNode &node) override;
    void visit(UnaryExpressionNode &node) override;
    void visit(BinaryExpressionNode &node) override;

    void visit(TypeDeclarationNode &node) override;
    void visit(ArrayTypeNode &node) override;
    void visit(BasicTypeNode &node) override;
    void visit(RecordTypeNode &node) override;

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

    void assertUnique(Identifier *ident, Node &node);
    bool assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual);
    void checkExport(DeclarationNode &node);

    std::unique_ptr<LiteralNode> fold(const ExpressionNode *expr) const;
    int foldNumber(const ExpressionNode *expr) const;
    bool foldBoolean(const ExpressionNode *expr) const;
    std::string foldString(const ExpressionNode *expr) const;

    TypeNode *resolveType(TypeNode *type);

public:
    explicit SemanticAnalysis(SymbolTable *symtab);
    ~SemanticAnalysis() override = default;

    void run(Logger *logger, Node *node) override;

};


#endif //OBERON_LLVM_SEMANTICANALYSIS_H
