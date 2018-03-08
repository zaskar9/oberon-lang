/*
 * Header file of the AST module nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H


#include <memory>
#include <string>
#include "BlockNode.h"

class ModuleNode final : public BlockNode {

private:
    std::string name_;

public:
    explicit ModuleNode(FilePos pos, const std::string &name);
    ~ModuleNode() final;

    const std::string getName() const;

    virtual void print(std::ostream &stream) final;

};


#endif //OBERON0C_MODULENODE_H
