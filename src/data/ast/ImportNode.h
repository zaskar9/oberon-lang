//
// Created by Michael Grossniklaus on 3/6/22.
//

#ifndef OBERON_LANG_IMPORTNODE_H
#define OBERON_LANG_IMPORTNODE_H


#include "Node.h"
#include "Ident.h"

class ImportNode : public Node {

private:
    std::unique_ptr<Ident> alias_;
    std::unique_ptr<Ident> module_;

public:
    explicit ImportNode(const FilePos &pos, std::unique_ptr<Ident> alias, std::unique_ptr<Ident> module) :
            Node(NodeType::import, pos), alias_(std::move(alias)), module_(std::move(module)) {};
    ~ImportNode() override = default;

    [[nodiscard]] Ident* getAlias() const;
    [[nodiscard]] Ident* getModule() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON_LANG_IMPORTNODE_H
