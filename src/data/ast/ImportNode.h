//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IMPORTNODE_H
#define OBERON_LANG_IMPORTNODE_H


#include "Node.h"
#include "Identifier.h"

class ModuleNode;

class ImportNode : public Node {

private:
    std::unique_ptr<Identifier> alias_;
    std::unique_ptr<ModuleNode> module_;

public:
    explicit ImportNode(const FilePos &pos, std::unique_ptr<Identifier> alias, std::unique_ptr<Identifier> name);
    ~ImportNode() override;

    [[nodiscard]] Identifier* getAlias() const;
    [[nodiscard]] ModuleNode* getModule() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON_LANG_IMPORTNODE_H
