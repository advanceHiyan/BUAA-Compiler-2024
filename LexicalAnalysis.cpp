//
// Created by aba15 on 2024/9/22.
//

#include "LexicalAnalysis.h"
#include "ConstType.h"
#include <cctype>
#include <cassert>
#include <sstream>
#include <iostream>

Lexer::Lexer(std::ifstream &input, const std::unordered_map<ReservedWord, ConstType>& reserveToConstMap)
    : input(input), reserveToConstMap(reserveToConstMap) {
    std::stringstream buffer;
    buffer << input.rdbuf(); // 读取文件内容到 stringstream
    content = buffer.str(); // 转换为 string
    curPos = 0;
}

std::string Lexer::readIdentifierOrKeyword() {
    std::string token;
    while (curPos < content.size() && std::isalnum(content[curPos]) || content[curPos] == '_') {
        token += content[curPos++];
    }
    return token;
}

std::string Lexer::readIntegerConstant() {
    std::string token;
    while (curPos < content.size() && std::isdigit(content[curPos])) {
        token += content[curPos++];
    }
    return token;
}

std::string Lexer::readStringConstant() {
    std::string token;
    token += content[curPos++];
    while (curPos < content.size() && content[curPos] != '\"') {
        if (content[curPos] == '\\') {
            token += content[curPos++];
        }
        token += content[curPos++];
    }
    token += content[curPos++];
    return token;
}

std::string Lexer::readCharacterConstant() {
    std::string token;
    token += content[curPos++];
    if (curPos + 1 < content.size()) {
        if (content[curPos] == '\\') {
            token += content[curPos++];
        }
        token += content[curPos++];
    }
    token += content[curPos++];
    return token;
}

int Lexer::next() {
    goSpacetoNext();

    if (curPos >= content.size()) {
        curLexType = ConstType::OVERFILE; // 文件结束
        curToken = "";
        return -1;
    }

    char ch = content[curPos];

    if (std::isalpha(ch) || ch == '_') {
        curToken = readIdentifierOrKeyword();
        auto reserve = strToReserveMap.find(curToken);
        if (reserve != strToReserveMap.end()) {
            auto it = reserveToConstMap.find(reserve -> second);
            curLexType = it->second;
        }
        else {
            curLexType = ConstType::IDENFR; // 未知关键字
        }
    }
    else if (std::isdigit(ch)) {
        curToken = readIntegerConstant();
        curLexType = ConstType::INTCON; // 整数常量
    }
    else if (ch == '\"') {
        curToken = readStringConstant();
        curLexType = ConstType::STRCON; // 字符串常量
    }
    else if (ch == '\'') {
        curToken = readCharacterConstant();
        curLexType = ConstType::CHRCON; // 字符常量
        return 0;
    }
    else if (ch == '<' && curPos + 1 < content.size() && content[curPos + 1] == '=') {
        curToken = "<=";
        curLexType = ConstType::LEQ; // 小于等于
        curPos += 2;
    }
    else if (ch == '>' && curPos + 1 < content.size() && content[curPos + 1] == '=') {
        curToken = ">=";
        curLexType = ConstType::GEQ; // 大于等于
        curPos += 2;
    }
    else if (ch == '!' && curPos + 1 < content.size() && content[curPos + 1] == '=') {
        curToken = "!=";
        curLexType = ConstType::NEQ; // 不等于
        curPos += 2;
    }
    else if (ch == '=' && curPos + 1 < content.size() && content[curPos + 1] == '=' ) {
        curToken = "==";
        curLexType = ConstType::EQL; // 等于等于
        curPos += 2;
    }
    else if(ch == '&') {
        if(curPos + 1 < content.size() && content[curPos + 1] == '&') {
            curToken = "&&";
            curLexType = ConstType::AND; // 逻辑与
            curPos += 2;
        }
        else {
            curToken = "&";
            curLexType = ConstType::SINGLEAND; //
            curPos++;
        }
    }
    else if(ch == '|') {
        if(curPos + 1 < content.size() && content[curPos + 1] == '|') {
            curToken = "||";
            curLexType = ConstType::OR; // 逻辑或
            curPos += 2;
            }
        else {
            curToken = "|";
            curLexType = ConstType::SINGLEOR; //
            curPos++;
        }
    }
    else {
        curToken = ch;
        auto it = charToConstMap.find(ch);
        curLexType = it->second;
        curPos++;
    }
    return 0;
}

void Lexer:: goSpacetoNext() {
    while (curPos < content.size() && std::isspace(content[curPos])) {
        if (content[curPos] == '\n') {
            lineNum++;
        }
        curPos++; // 忽略空白字符
    }

    if(content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '/') {
        // 注释
        while (curPos < content.size() && content[curPos] != '\n') {
            curPos++;
        }
        lineNum++;
        goSpacetoNext();
    }
    else if(content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '*') {
        // 注释
        curPos += 2;
        while (curPos < content.size() && !(content[curPos] == '*' && curPos + 1 < content.size() && content[curPos + 1] == '/')) {
            if (content[curPos] == '\n') {
                lineNum++;
            }
            curPos++;
        }
        curPos += 2;
        goSpacetoNext();
    }
    return ;
}

std::string Lexer::getToken() const {
    return curToken;;
}

ConstType Lexer::getLexType() const {
    // 这里可以返回当前 token 的类型，但在 next() 方法中已经处理了
    return curLexType; // 示例返回值，实际不需要这个方法
}

int Lexer::getLineNum() const {
    return lineNum;
}
