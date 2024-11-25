//
// Created by aba15 on 2024/10/15.
//

#ifndef COMPILER_SEMANTICVISITOR_H
#define COMPILER_SEMANTICVISITOR_H

#include "../parser/Node.h"
#include "SymbolTable.h"
#include "Visitor.h"

class SemanticAnalyzer : public Visitor { // 语义分析
public:
    void visit(Node *node) override; // 重载visit函数，实现语义分析

protected:
    OverallSymbolTable *overall_table; // 全局符号表
    int block_num = 1;//记录用了多少个块号，只用于建立新符号表，不可用于其它参数
    std::vector<Symbol*> printf_list; // 用于记录待输出语句及其块号

    void visit_Decl(Node *node, SymbolTable *this_table);

    void visit_def(Node *node, SymbolTable *this_table, std::string char_or_int);

    void visit_FuncDef(Node *func_def);

    void visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table);

    void visit_Block(Node *block, SymbolTable *this_table);

    void visit_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_If_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_For_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_LVal(Node *lval, SymbolTable *this_table);

    void detecting_printf_errors(Node *stmt, SymbolTable *this_table);

    void visit_Cond(Node *cond, SymbolTable *this_table);

    void visit_Exp_or_ConstExp(Node *exp, SymbolTable *this_table);

    void visit_InitVal(Node *init_val, SymbolTable *this_table);

    void visit_AddExp(Node *add_exp, SymbolTable *this_table);

    void visit_MulExp(Node *mulexp, SymbolTable *this_table);

    void visit_Unary(Node *unary, SymbolTable *this_table);

    void visit_PrimaryExp(Node *primary_exp, SymbolTable *this_table);

    void visit_LOrExp(Node *lor_exp, SymbolTable *this_table);

    void visit_LAndExp(Node *land_exp, SymbolTable *this_table);

    void visit_EqExp(Node *eq_exp, SymbolTable *this_table);

    void visit_RelExp(Node *rel_exp, SymbolTable *this_table);

    void visit_LVal_without_assign(Node *lval, SymbolTable *this_table);

    void detectingFunReturnErrors(Node *block, SymbolTable *this_table);

    void detectingFunRParamsErrors(Node *unaryExp, int line_num, std::vector<SymbolType> *params
                                   ,SymbolTable * this_table);

    int getExpFunType(Node *exp, SymbolTable *this_table);
};

#endif //COMPILER_SEMANTICVISITOR_H
