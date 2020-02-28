/*
 * Header file of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "../../util/Logger.h"

enum class NodeType : char {
    module, procedure, statement_sequence,
    unary_expression, binary_expression,
    record_type, array_type, basic_type,
    field, parameter, variable, constant, type_declaration,
    integer, boolean, string,
    name_reference,
    assignment, loop, while_loop, repeat_loop, for_loop, if_then_else, else_if, procedure_call, ret
};

class NodeVisitor;

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;

public:
    explicit Node(NodeType nodeType, FilePos pos);
    virtual ~Node() = 0;
    
    NodeType getNodeType() const;
    const FilePos getFilePos() const;

    virtual void accept(NodeVisitor &visitor) = 0;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);

};

#endif //OBERON0C_AST_H
