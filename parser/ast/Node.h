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
    module, procedure, statement_sequence,
    unary_expression, binary_expression,
    record_type, array_type, basic_type,
    field, parameter, variable, constant,
    number, boolean, string,
    type_reference, name_reference,
    assignment, while_loop, if_then_else, procedure_call
};

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;

public:
    explicit Node(NodeType nodeType, FilePos pos);
    virtual ~Node() = 0;
    
    NodeType getNodeType() const;
    const FilePos getFilePos() const;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);

};

#endif //OBERON0C_AST_H
