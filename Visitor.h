//
// Created by aba15 on 2024/10/15.
//

#ifndef COMPILER_VISITOR_H
#define COMPILER_VISITOR_H


#include "Node.h"
#include "SymbolTable.h"

class Visitor { // 遍历树，不论度是多少，都能共享Visitor类空间
    public:
        virtual void visit(Node* node) = 0; // 定义一个虚函数，用于遍历树
    protected:
        SymbolTable* curTable; // 符号表

    private:
};


class SemanticAnalyzer : public Visitor { // 语义分析
    public:
        void visit(Node *node) override; // 重载visit函数，实现语义分析
    private:
        std::vector<Symbol*> printf_list; // 用于记录待输出语句及其块号
    void visit_Decl(Node *node, SymbolTable *this_table);
    int block_num = 1;//记录用了多少个块号，只用于建立新符号表，不可用于其它参数
    void visit_def(Node *node, SymbolTable *this_table, std::string char_or_int);

    void visit_FuncDef(Node *func_def, SymbolTable *this_table);

    void visit_FParamsAndBlock(Node *func_def, SymbolTable *this_table);

    void visit_Block(Node *block, SymbolTable *this_table);

    void visit_Stmt(Node *stmt, SymbolTable *this_table);
};

#endif //COMPILER_VISITOR_H
