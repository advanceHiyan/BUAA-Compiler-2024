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
    // Value1: Ident_name（变量标识符名）, Value2: 空指针

    // 将值（可以是变量的值或数字）压入栈中
    PUSHINT, PUSHCHAR,
    // Value1: Ident_name（变量标识符名）或Digit（数字）, Value2: 空指针

    POPLVAL,

    // 加法操作，将两个值相加
    ADD,SUB,

    // 乘法操作，将两个值相乘
    MULT,DIV,MOD,

    // 逻辑非操作，对一个布尔值取反
    NOT,MINU,
    // Value1:!（逻辑非操作符）, Value2: 空指针

    // 主函数标签，用于标识程序的主函数开始位置
    MAIN,
    // Value1: 空指针, Value2: 空指针

    // 函数标签，用于标识一个函数的开始位置
    FUNC,
    // Value1: 空指针, Value2: 空指针

    // 函数返回，可以选择是否返回一个值
    RET,
    // Value1: Return value or not（是否返回值）, Value2: 空指针

    // 定义函数参数，包括参数名和参数类型
    PARINT,PARCHAR,PARINTARRAY,PARCHARARRAY,


    // 函数调用，调用指定名称的函数
    CALL,
    // Value1: Function name（函数名）, Value2: 空指针

    // 准备函数调用的参数，指定参数类型
    APINT,  APINTARRAY,
    // Value1: Type（参数类型）, Value2: 空指针

    // 获取一个整数并将其压入栈顶
    GETINT,
    // Value1: 空指针, Value2: 空指针

    GETCHAR,

    // 弹出栈中的值并打印，可能涉及字符串和参数数量
    PRINTF,
    // Value1: String（要打印的字符串）, Value2: Para num（参数数量）

    // 获取变量的值，指定变量名和变量类型
    VALUE,
    // Value1: Ident_name（变量标识符名）, Value2: Type（变量类型）

    // 获取变量的地址，指定变量名和变量类型
    ADDRESS,
    // Value1: Ident_name（变量标识符名）, Value2: Type（变量类型）

    // 推送一些东西来占位
    PLACESPACE,
    // Value1: 空指针, Value2: 空指针

    OVERFUN,

    // 退出程序
    EXIT,
    // Value1: 空指针, Value2: 空指针

};

const std::unordered_map<CodeType, std::string> codeTypeToStringMap = {
        {CodeType::LABEL, "LABEL"},
        {CodeType::VARINT, "VARINT"},
        {CodeType::VARCHAR, "VARCHAR"},
        {CodeType::VARINTARRAY, "VARINTARRAY"},
        {CodeType::VARCHARARRAY, "VARCHARARRAY"},
        {CodeType::PUSHINT, "PUSHINT"},
        {CodeType::PUSHCHAR, "PUSHCHAR"},
        {CodeType::POPLVAL, "POPLVAL"},
        {CodeType::ADD, "ADD"},
        {CodeType::SUB, "SUB"},
        {CodeType::MULT, "MULT"},
        {CodeType::DIV, "DIV"},
        {CodeType::MOD, "MOD"},
        {CodeType::NOT, "NOT"},
        {CodeType::MINU, "MINU"},
        {CodeType::MAIN, "MAIN"},
        {CodeType::FUNC, "FUNC"},
        {CodeType::RET, "RET"},
        {CodeType::PARINT, "PARINT"},
        {CodeType::PARCHAR, "PARCHAR"},
        {CodeType::PARINTARRAY, "PARINTARRAY"},
        {CodeType::PARCHARARRAY, "PARCHARARRAY"},
        {CodeType::CALL, "CALL"},
        {CodeType::APINT, "APINT"},
        {CodeType::APINTARRAY, "APINTARRAY"},
        {CodeType::GETINT, "GETINT"},
        {CodeType::GETCHAR, "GETCHAR"},
        {CodeType::PRINTF, "PRINTF"},
        {CodeType::VALUE, "VALUE"},
        {CodeType::ADDRESS, "ADDRESS"},
        {CodeType::PLACESPACE, "PLACESPACE"},
        {CodeType::EXIT, "EXIT"},
        {CodeType::OVERFUN, "OVERFUN"}
};

#endif //COMPILER_PCODETYPE_H
