/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H


#include <memory>
#include <string>
#include "BlockNode.h"
#include "ProcedureNode.h"

class ModuleNode final : public BlockNode {

private:
    std::string name_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;

public:
    explicit ModuleNode(const FilePos &pos, std::string name, int level) :
            BlockNode(NodeType::module, pos, level), name_(std::move(name)), procedures_() { };
    ~ModuleNode() final = default;

    [[nodiscard]] std::string getName() const;

    void addProcedure(std::unique_ptr<ProcedureNode> procedure) final;
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const final;
    [[nodiscard]] size_t getProcedureCount() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_MODULENODE_H
