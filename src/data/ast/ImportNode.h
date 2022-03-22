//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IMPORTNODE_H
#define OBERON_LANG_IMPORTNODE_H


#include "Node.h"
#include "Identifier.h"

class ImportNode : public Node {

private:
    std::unique_ptr<Identifier> alias_;
    std::unique_ptr<Identifier> module_;

public:
    explicit ImportNode(const FilePos &pos, std::unique_ptr<Identifier> alias, std::unique_ptr<Identifier> module) :
            Node(NodeType::import, pos), alias_(std::move(alias)), module_(std::move(module)) {};
    ~ImportNode() override = default;

    [[nodiscard]] Identifier* getAlias() const;
    [[nodiscard]] Identifier* getModule() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON_LANG_IMPORTNODE_H
