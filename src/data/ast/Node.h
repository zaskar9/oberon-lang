/*
 * Base class for all AST nodes used in the Oberon LLVM compiler.
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
#include "../../global.h"

enum class NodeType : char {
    ident,
    module, import, procedure, statement_sequence,
    unary_expression, binary_expression,
    array_type, basic_type, pointer_type, procedure_type, record_type,
    field, parameter, variable, constant, type_declaration,
    integer, real, boolean, string, pointer,
    value_reference, type_reference,
    assignment, loop, while_loop, repeat_loop, for_loop, if_then_else, else_if, procedure_call, ret
};

class NodeVisitor;

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;

protected:
    void setNodeType(const NodeType nodeType);

public:
    explicit Node(const NodeType nodeType, const FilePos &pos) : nodeType_(nodeType), pos_(pos) { };
    virtual ~Node();

    [[nodiscard]] NodeType getNodeType() const;
    [[nodiscard]] FilePos pos() const;

    virtual void accept(NodeVisitor &visitor) = 0;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);

};


#endif //OBERON0C_AST_H
