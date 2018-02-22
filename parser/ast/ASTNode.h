/*
 * Header file of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include <list>
#include <string>
#include <ostream>
#include "../../util/Logger.h"

enum class NodeType : char {
    unary_expression, binary_expression, boolean_constant, number_constant, string_constant
};

class ASTNode {

private:
    NodeType type_;
    FilePos pos_;

public:
    explicit ASTNode(NodeType type, FilePos pos);
    virtual ~ASTNode() = 0;
    
    const NodeType getNodeType() const;
    const FilePos getFilePos() const;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const ASTNode &node);

};

#endif //OBERON0C_AST_H
