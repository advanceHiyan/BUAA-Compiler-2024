// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

//
// Created by aba15 on 2024/11/22.
//

#ifndef COMPILER_PCODE_H
#define COMPILER_PCODE_H

#include <string>
#include "PcodeType.h"
#include <iostream>

struct PCode {
    CodeType type;
    void* x;
    void* y;
    std::string codeStr;

    // 构造函数
    PCode(CodeType type) : type(type), x(nullptr), y(nullptr) {}

    PCode(CodeType type, void* x) : type(type), x(x), y(nullptr) {

    }

    PCode(CodeType type, void* x, void* y) : type(type), x(x), y(y) {}

    void setCodeStr(std::string codeStr) {
        this->codeStr = codeStr;
    }

    std::string getCodeStr() const {
        return codeStr;
    }

    // 设置y的函数
    void setValue2(void* y) {
        this->y = y;
    }

    // 获取类型的函数
    CodeType getType() const {
        return type;
    }

    // 获取x的函数
    void* getValue1() const {
        return x;
    }

    // 获取y的函数
    void* getValue2() const {
        return y;
    }
};
#endif //COMPILER_PCODE_H
