//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IMPORTNODE_H
#define OBERON_LANG_IMPORTNODE_H


#include "Node.h"

class ModuleNode;

class ImportNode : public Node {

private:
    std::string alias_;
    std::unique_ptr<ModuleNode> module_;

public:
    explicit ImportNode(const FilePos &pos, std::string alias, std::string name);
    ~ImportNode() override;

    [[nodiscard]] std::string getAlias() const;
    [[nodiscard]] ModuleNode* getModule() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON_LANG_IMPORTNODE_H
