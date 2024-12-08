// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

//
// Created by aba15 on 2024/11/22.
//

#ifndef COMPILER_VISITOR_H
#define COMPILER_VISITOR_H

#include "SymbolTable.h"
#include "../parser/Node.h"

class  Visitor { // 遍历树，不论度是多少，都能共享Visitor类空间
public:
    virtual void visit(Node* node) = 0; // 定义一个虚函数，用于遍历树 ,=0表示函数在基类没有定义，所有派生类必须实现该函数
};

#endif //COMPILER_VISITOR_H
