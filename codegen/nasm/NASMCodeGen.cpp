/*
 * Simple tree-walk code generator to produce NASM assembly for the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "NASMCodeGen.h"

NASMCodeGen::NASMCodeGen(Assembly *assembly) : assembly_(assembly), registers_(), free_(), lValue_(false) {
    registers_.push_back(std::make_unique<Register>(0, "rax", "eax", "ax", "al"));
    registers_.push_back(std::make_unique<Register>(1, "rcx", "ecx", "cx", "cl"));
    registers_.push_back(std::make_unique<Register>(2, "rdx", "edx", "dx", "dl"));
    registers_.push_back(std::make_unique<Register>(3, "rbx", "ebx", "bx", "bl"));
    registers_.push_back(std::make_unique<Register>(4, "rsi", "esi", "si", "sil"));
    registers_.push_back(std::make_unique<Register>(5, "rdi", "edi", "di", "dil"));
    registers_.push_back(std::make_unique<Register>(6, "rsp", "esp", "sp", "spl"));
    registers_.push_back(std::make_unique<Register>(7, "rbp", "ebp", "bp", "bpl"));
    registers_.push_back(std::make_unique<Register>(8, "r8", "r8d", "r8w", "r8b"));
    registers_.push_back(std::make_unique<Register>(9, "r9", "r9d", "r9w", "r9b"));
    registers_.push_back(std::make_unique<Register>(10, "r10", "r10d", "r10w", "r10b"));
    registers_.push_back(std::make_unique<Register>(11, "r11", "r11d", "r11w", "r11b"));
    registers_.push_back(std::make_unique<Register>(12, "r12", "r12d", "r12w", "r12b"));
    registers_.push_back(std::make_unique<Register>(13, "r13", "r13d", "r13w", "r13b"));
    registers_.push_back(std::make_unique<Register>(14, "r14", "r14d", "r14w", "r14b"));
    registers_.push_back(std::make_unique<Register>(15, "r15", "r15d", "r15w", "r15b"));
    for (size_t i = 0; i < sizeof(free_); i++) {
        free_[i] = true;
    }
}

const Register* NASMCodeGen::allocRegister() {
    for (size_t i = 0; i < sizeof(free_); i++) {
        if (free_[i]) {
            free_[i] = false;
            return registers_.at(i).get();
        }
    }
    return nullptr;
}

void NASMCodeGen::freeRegister(const Register *reg) {
    free_[reg->getId()] = true;
}

OpMode NASMCodeGen::getMode(const TypeNode* type) const {
    if (type == BasicTypeNode::INTEGER) {
        return OpMode::d32;
    }
    if (type == BasicTypeNode::BOOLEAN) {
        return OpMode::b8;
    }
    return OpMode::q64;
}

void NASMCodeGen::visit(ModuleNode& node) {
    // reserve space for global variables
    auto section = assembly_->getBssSection();
    for (size_t i = 0; i < node.getVariableCount(); i++) {
        auto var = node.getVariable(i);
        auto ins = std::make_unique<ImmediateInstruction>(OpCode::res, OpMode::b8, nullptr, var->getType()->getSize());
        section->addInstruction(var->getName(), std::move(ins));
    }
    // generate code for procedures
    for (size_t i = 0; i < node.getProcedureCount(); i++) {
        node.getProcedure(i)->accept(*this);
    }
    // generate code for main
    node.getStatements()->accept(*this);
}

void NASMCodeGen::visit(ProcedureNode& node) { }

void NASMCodeGen::visit(ReferenceNode& node) {
    auto ref = node.dereference();
    int level = ref->getLevel();
    if (level == 1) /* global level */ {
        auto type = ref->getType();
        if (type->getNodeType() == NodeType::array_type) {
            // todo handle array access
        } else if (type->getNodeType() == NodeType::record_type) {
            // todo handle record access
        } else {
            auto reg = allocRegister();
            auto label = std::make_unique<LabelAddress>(assembly_->getLabel(ref->getName()));
            std::unique_ptr<AddressInstruction> ins;
            ins = std::make_unique<AddressInstruction>(OpCode::lea, OpMode::q64, reg, std::move(label), false);
            assembly_->getTextSection()->addInstruction(std::move(ins));
            result_ = reg;
            if (!lValue_) {
                auto mode = getMode(type);
                reg = allocRegister();
                ins = std::make_unique<AddressInstruction>(OpCode::mov, mode, reg, std::make_unique<EffectiveAddress>(result_), false);
                assembly_->getTextSection()->addInstruction(std::move(ins));
                freeRegister(result_);
            }
            result_ = reg;
        }
    }
}

void NASMCodeGen::visit(ConstantDeclarationNode& node) { }

void NASMCodeGen::visit(FieldNode& node) { }

void NASMCodeGen::visit(ParameterNode& node) { }

void NASMCodeGen::visit(VariableDeclarationNode& node) { }

void NASMCodeGen::visit(BooleanLiteralNode& node) {
    result_ = allocRegister();
}

void NASMCodeGen::visit(IntegerLiteralNode& node) {
    result_ = allocRegister();
    auto mode = getMode(node.getType());
    auto ins = std::make_unique<ImmediateInstruction>(OpCode::mov, mode, result_, node.getValue());
    assembly_->getTextSection()->addInstruction(std::move(ins));
}

void NASMCodeGen::visit(StringLiteralNode& node) {
    result_ = allocRegister();
}

void NASMCodeGen::visit(FunctionCallNode& node) { }

void NASMCodeGen::visit(UnaryExpressionNode& node) { }

void NASMCodeGen::visit(BinaryExpressionNode& node) {
    node.getLeftExpression()->accept(*this);
    const Register* rLeft = result_;
    node.getRightExpression()->accept(*this);
    const Register* rRight = result_;
    auto op = OpCode::nop;
    switch (node.getOperator()) {
        case OperatorType::PLUS: op = OpCode::add; break;
        case OperatorType::MINUS: op = OpCode::sub; break;
        case OperatorType::TIMES: op = OpCode::imul; break;
        case OperatorType::DIV: op = OpCode::idiv; break;
    }
    auto mode = getMode(node.getType());
    auto ins = std::make_unique<RegisterInstruction>(op, mode, rLeft, rRight);
    assembly_->getTextSection()->addInstruction(std::move(ins));
    freeRegister(rRight);
    result_ = rLeft;
}

void NASMCodeGen::visit(TypeDeclarationNode& node) { }

void NASMCodeGen::visit(ArrayTypeNode& node) { }

void NASMCodeGen::visit(BasicTypeNode& node) { }

void NASMCodeGen::visit(RecordTypeNode& node) { }

void NASMCodeGen::visit(StatementSequenceNode& node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        node.getStatement(i)->accept(*this);
    }
}

void NASMCodeGen::visit(AssignmentNode& node) {
    node.getRvalue()->accept(*this);
    const Register* rRvalue = result_;
    lValue_ = true;
    node.getLvalue()->accept(*this);
    lValue_ = false;
    const Register* rLvalue = result_;
    auto mode = getMode(node.getLvalue()->getType());
    auto ins = std::make_unique<AddressInstruction>(OpCode::mov, mode, rRvalue, std::make_unique<EffectiveAddress>(rLvalue), true);
    assembly_->getTextSection()->addInstruction(std::move(ins));
    freeRegister(rRvalue);
    freeRegister(rLvalue);
}

void NASMCodeGen::visit(IfThenElseNode& node) { }

void NASMCodeGen::visit(ElseIfNode& node) { }

void NASMCodeGen::visit(ProcedureCallNode& node) { }

void NASMCodeGen::visit(LoopNode& node) { }

void NASMCodeGen::visit(WhileLoopNode& node) { }

void NASMCodeGen::visit(RepeatLoopNode& node) { }

void NASMCodeGen::visit(ForLoopNode& node) { }

void NASMCodeGen::visit(ReturnNode& node) { }