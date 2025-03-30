/*
 * Base class for all AST nodes used in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include "global.h"
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

enum class NodeType : char {
    ident,
    module, import, procedure, statement_sequence,
    unary_expression, binary_expression, range_expression, set_expression, qualified_expression,
    array_type, basic_type, pointer_type, procedure_type, record_type,
    field, parameter, variable, constant, type,
    integer, real, boolean, string, character, pointer, range, set,
    assignment, loop, while_loop, repeat_loop, for_loop, if_then_else, else_if,
    case_of, case_label, case_case, ret,
    qualified_statement
};

class NodeVisitor;

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;

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
