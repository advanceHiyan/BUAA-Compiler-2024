//
// Created by aba15 on 2024/11/22.
//

#ifndef COMPILER_TOPCODEVISITOR_H
#define COMPILER_TOPCODEVISITOR_H


#include "../semantic/SymbolTable.h"
#include "../parser/Node.h"
#include "../semantic/Visitor.h"
#include "./Pcode.h"

class ToPcodeVisitor : public Visitor {
public:
    void visit(Node *node) override; // 重载visit函数，实现代码生成
    std :: vector<PCode*>* getPcodeList(); // 获取生成的pcode语句
protected:
    OverallSymbolTable *overall_table; // 全局符号表
    int block_num = 1;//记录用了多少个块号，只用于建立新符号表，不可用于其它参数
    std :: vector<PCode*>* pCodeList = new std :: vector<PCode*>; // 存放生成的pcode语句,不初始化会报错

    void visit_Decl(Node *node, SymbolTable *this_table);

    void visit_def(Node *node, SymbolTable *this_table, std::string char_or_int);

    void visit_FuncDef(Node *func_def);

    void visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table);

    void visit_Block(Node *block, SymbolTable *this_table);

    void visit_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_If_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_For_Stmt(Node *stmt, SymbolTable *this_table);

    void visit_LVal(Node *lval, SymbolTable *this_table);

    void visit_Cond(Node *cond, SymbolTable *this_table);

    void visit_Exp_or_ConstExp(Node *exp, SymbolTable *this_table);

    void visit_AddExp(Node *add_exp, SymbolTable *this_table);

    void visit_MulExp(Node *mulexp, SymbolTable *this_table);

    void visit_Unary(Node *unary, SymbolTable *this_table);

    void visit_PrimaryExp(Node *primary_exp, SymbolTable *this_table);

    void visit_LOrExp(Node *lor_exp, SymbolTable *this_table);

    void visit_LAndExp(Node *land_exp, SymbolTable *this_table);

    void visit_EqExp(Node *eq_exp, SymbolTable *this_table);

    void visit_RelExp(Node *rel_exp, SymbolTable *this_table);

    void visit_LVal_without_assign(Node *lval, SymbolTable *this_table);

    int getExpFunType(Node *exp, SymbolTable *this_table);

    void addCode(CodeType type, std::string value);

    void addCode(CodeType type, int value, int value2);

    void addCode(CodeType type);

    void addCode(CodeType type, int value);

    void printPcode();

    void addCode(CodeType type, std::string value, int value2);

    void visit_InitVal(Node *init_val, SymbolTable *this_table, std::string name);
};


#endif //COMPILER_TOPCODEVISITOR_H
