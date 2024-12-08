// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

//
// Created by aba15 on 2024/11/22.
//

#ifndef COMPILER_PCODETYPE_H
#define COMPILER_PCODETYPE_H

#include <unordered_map>

enum  class CodeType {
    LABEL, //标签
    VARINT, VARCHAR, VARINTARRAY, VARCHARARRAY, // 声明一个变量
    LDI, LDC,// 将值数字、char压入栈顶
    PAL,  //popValue then popAddress then setValue to address
    ADD,SUB, //加减
    MUL,DIV,MOD, //乘除、取模
    NOT,MUS, //取否 取负
    MAIN, // main标签
    FUNC, // 函数标签
    RET, //函数返回
    PARINT,PARCHAR,PARINTARRAY,PARCHARARRAY,// 定义函数参数
    CAL, // 函数调用，调用指定名称的函数
    APR,  APA,    // 传递参数、传递数组参数
    GETINT, GETCHAR, // 输入
    PRINTF,// 输出
    LOD, // 获取变量的值，指定变量名和变量类型
    LDA,// 获取变量的地址，指定变量名和变量类型
    EMPTY,// 给数组空间占位
    EXF,//标记函数结束，用于无返回值的void函数
    EXIT,// 退出程序
    JMP, //无条件跳转
    JPC, //如果栈顶为0，跳转
    JPF, //如果栈顶为非0，跳转
    OR,AND, //逻辑运算
    EQL, //判断两个值是否相等
    NEQ, //判断两个值是否不相等
    LSS, //判断第一个值是否小于第二个值
    LER, //判断第一个值是否小于等于第二个值
    GRT, //判断第一个值是否大于第二个值
    GEQ, //判断第一个值是否大于等于第二个值
};

const std::unordered_map<CodeType, std::string> codeTypeToStringMap = {
        {CodeType::LABEL, "LABEL"},
        {CodeType::VARINT, "VARINT"},
        {CodeType::VARCHAR, "VARCHAR"},
        {CodeType::VARINTARRAY, "VARINTARRAY"},
        {CodeType::VARCHARARRAY, "VARCHARARRAY"},
        {CodeType::LDI, "LDI"},
        {CodeType::LDC, "LDC"},
        {CodeType::PAL, "PAL"},
        {CodeType::ADD, "ADD"},
        {CodeType::SUB, "SUB"},
        {CodeType::MUL, "MUL"},
        {CodeType::DIV, "DIV"},
        {CodeType::MOD, "MOD"},
        {CodeType::NOT, "NOT"},
        {CodeType::MUS, "MUS"},
        {CodeType::MAIN, "MAIN"},
        {CodeType::FUNC, "FUNC"},
        {CodeType::RET, "RET"},
        {CodeType::PARINT, "PARINT"},
        {CodeType::PARCHAR, "PARCHAR"},
        {CodeType::PARINTARRAY, "PARINTARRAY"},
        {CodeType::PARCHARARRAY, "PARCHARARRAY"},
        {CodeType::CAL, "CAL"},
        {CodeType::APR, "APR"},
        {CodeType::APA, "APA"},
        {CodeType::GETINT, "GETINT"},
        {CodeType::GETCHAR, "GETCHAR"},
        {CodeType::PRINTF, "PRINTF"},
        {CodeType::LOD, "LOD"},
        {CodeType::LDA, "LDA"},
        {CodeType::EMPTY, "EMPTY"},
        {CodeType::EXIT, "EXIT"},
        {CodeType::EXF, "EXF"},
        {CodeType::JMP, "JMP"},
        {CodeType::JPC, "JPC"},
        {CodeType::OR, "OR"},
        {CodeType::AND, "AND"},
        {CodeType::EQL, "EQL"},
        {CodeType::NEQ, "NEQ"},
        {CodeType::LSS, "LSS"},
        {CodeType::LER, "LER"},
        {CodeType::GRT, "GRT"},
        {CodeType::GEQ, "GEQ"},
        {CodeType::JPF, "JPF"}
};

#endif //COMPILER_PCODETYPE_H
