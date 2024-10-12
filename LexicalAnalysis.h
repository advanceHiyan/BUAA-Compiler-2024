//
// Created by aba15 on 2024/9/23.
//

#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H

#include <fstream>
#include <string>
#include <unordered_map>
#include "ConstType.h"

class Lexer {
private:
    std::ifstream &input;
    std::string content;
    size_t curPos = 0; // 当前字符位置
    std::string curToken; // 当前词法单元
    ConstType curLexType; // 当前词法单元类型
    int lineNum = 1; // 当前行号
    const std::unordered_map<ReservedWord, ConstType>& reserveToConstMap;

    std::string readIdentifierOrKeyword();
    std::string readIntegerConstant();
    std::string readStringConstant();
    std::string readCharacterConstant();
    void goSpacetoNext();

public:
    Lexer(std::ifstream &input, const std::unordered_map<ReservedWord, ConstType> &reserveToConstMap);
    int next();
    std::string getToken() const;
    ConstType getLexType() const;
    int getLineNum() const;
};

#endif // LEXICALANALYSIS_H
