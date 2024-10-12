//
// Created by aba15 on 2024/9/23.
//

#ifndef COMPILER_CONSTTYPE_H
#define COMPILER_CONSTTYPE_H

#include <unordered_map>
#include <string>

enum ConstType {
    IDENFR, // 非法字符
    INTCON, // intConst
    STRCON, // stringConst
    CHRCON, // charConst
    MAINTK, // main
    CONSTTK, // const
    INTTK, // int
    CHARTK, // char
    BREAKTK, // break
    CONTINUETK, // continue
    IFTK, // if

    ELSETK, // else
    NOT, // !
    AND, // &&
    OR, // ||
    FORTK, // for
    GETINTTK, // getint
    GETCHARTK, // getchar
    PRINTFTK, // printf
    RETURNTK, // return
    PLUS, // +
    MINU, // -

    VOIDTK, // void
    MULT, // *
    DIV, // /
    MOD, // %
    LSS, // <
    LEQ, // <=
    GRE, // >
    GEQ, // >=
    EQL, // ==
    NEQ, //!=
    ASSIGN, // =

    SEMICN, // ;
    COMMA, // ,
    LPARENT, // (
    RPARENT,     // )
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // {
    RBRACE, // }
    OVERFILE, // overfile
    SINGLEAND, // &
    SINGLEOR, // |
};

enum class ReservedWord {
    VOIDTK, // void
    FORTK, // for
    GETINTTK, // getint
    GETCHARTK, // getchar
    PRINTFTK, // printf
    RETURNTK, // return
    MAINTK, // main
    CONSTTK, // const
    INTTK, // int
    CHARTK, // char
    BREAKTK, // break
    CONTINUETK, // continue
    IFTK, // if
    ELSETK, // else
};


// 定义哈希表，从字符映射到 ConstType
const std::unordered_map<char, ConstType> charToConstMap ={
        {'+', ConstType::PLUS},
        {'-', ConstType::MINU},
        {'*', ConstType::MULT},
        {'/', ConstType::DIV},
        {'%', ConstType::MOD},
        {'<', ConstType::LSS},
        {'>', ConstType::GRE},
        {'=', ConstType::ASSIGN},
        {'!', ConstType::NOT}, // 注意：'!' 实际上可能代表 '!='，需要额外逻辑判断
        {'&', ConstType::SINGLEAND}, // 注意：'&' 实际上可能代表 '&&'，需要额外逻辑判断
        {'|', ConstType::SINGLEOR}, // 注意：'|' 实际上可能代表 '||'，需要额外逻辑判断
        {';', ConstType::SEMICN},
        {',', ConstType::COMMA},
        {'(', ConstType::LPARENT},
        {')', ConstType::RPARENT},
        {'[', ConstType::LBRACK},
        {']', ConstType::RBRACK},
        {'{', ConstType::LBRACE},
        {'}', ConstType::RBRACE}
    };


const std::unordered_map<ReservedWord, ConstType> reserveToConstMap = {
        {ReservedWord::VOIDTK,     ConstType::VOIDTK},
        {ReservedWord::FORTK,      ConstType::FORTK},
        {ReservedWord::GETINTTK,   ConstType::GETINTTK},
        {ReservedWord::GETCHARTK,  ConstType::GETCHARTK},
        {ReservedWord::PRINTFTK,   ConstType::PRINTFTK},
        {ReservedWord::RETURNTK,   ConstType::RETURNTK},
        {ReservedWord::MAINTK,     ConstType::MAINTK},
        {ReservedWord::CONSTTK,    ConstType::CONSTTK},
        {ReservedWord::INTTK,      ConstType::INTTK},
        {ReservedWord::CHARTK,     ConstType::CHARTK},
        {ReservedWord::BREAKTK,    ConstType::BREAKTK},
        {ReservedWord::CONTINUETK, ConstType::CONTINUETK},
        {ReservedWord::IFTK,       ConstType::IFTK},
        {ReservedWord::ELSETK,     ConstType::ELSETK}
};

// 生成哈希表
const std::unordered_map<std::string, ReservedWord> strToReserveMap = {
        {"void", ReservedWord::VOIDTK},
        {"for", ReservedWord::FORTK},
        {"getint", ReservedWord::GETINTTK},
        {"getchar", ReservedWord::GETCHARTK},
        {"printf", ReservedWord::PRINTFTK},
        {"return", ReservedWord::RETURNTK},
        {"main", ReservedWord::MAINTK},
        {"const", ReservedWord::CONSTTK},
        {"int", ReservedWord::INTTK},
        {"char", ReservedWord::CHARTK},
        {"break", ReservedWord::BREAKTK},
        {"continue", ReservedWord::CONTINUETK},
        {"if", ReservedWord::IFTK},
        {"else", ReservedWord::ELSETK}
};

#endif //COMPILER_CONSTTYPE_H
