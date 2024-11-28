//
// Created by aba15 on 2024/11/22.
//

#ifndef COMPILER_PCODETYPE_H
#define COMPILER_PCODETYPE_H

#include <unordered_map>

enum  class CodeType {
    LABEL,

    // 声明一个变量，用于在程序中定义新的变量
    VARINT, VARCHAR, VARINTARRAY, VARCHARARRAY,

    // 将值数字、char压入栈顶
    LDI, LDC,

    PAL, //popValue then popAddress then setValue to address

    // 加法操作，将两个值相加
    ADD,SUB,

    // 乘法操作，将两个值相乘
    MUL,DIV,MOD,

    // 逻辑非操作，取负
    NOT,MUS,

    // 主函数标签，用于标识程序的主函数开始位置
    MAIN,

    // 函数标签，用于标识一个函数的开始位置
    FUNC,

    // 函数返回，可以选择是否返回一个值
    RET,

    // 定义函数参数，包括参数名和参数类型
    PARINT,PARCHAR,PARINTARRAY,PARCHARARRAY,

    // 函数调用，调用指定名称的函数
    CAL,

    // 传递参数、传递函数参数
    APR,  APA,

    // 获取一个整数并将其压入栈顶
    GETINT,

    GETCHAR,

    // 弹出栈中的值并打印，可能涉及字符串和参数数量
    PRINTF,

    // 获取变量的值，指定变量名和变量类型
    LOD,

    // 获取变量的地址，指定变量名和变量类型
    LDA,

    // 推送一些东西来占位
    EMPTY,

    //标记函数结束，用于无返回值的void函数
    EXF,

    // 退出程序
    EXIT,
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
        {CodeType::EXF, "EXF"}
};

#endif //COMPILER_PCODETYPE_H
