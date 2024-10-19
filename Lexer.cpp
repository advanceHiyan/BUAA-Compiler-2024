//
// Created by aba15 on 2024/9/22.
//

#include "Lexer.h"
#include "ConstType.h"
#include "Node.h"
#include <cctype>

Lexer::Lexer(std::string &content, const std::unordered_map<ReservedWord, ConstType>& reserveToConstMap)
    : content(content), reserveToConstMap(reserveToConstMap) {
    curPos = 0;
}

std::string Lexer::readIdentifierOrKeyword() {
    std::string token;
    std::string  copyToken;
    while (curPos < content.size() && std::isalnum(content[curPos]) || content[curPos] == '_') {
        char ch = content[curPos];//std::isalnum字母数字
        if (std::isalpha(ch)) { //字母
            copyToken += std::tolower(ch);
        }
        else {
            copyToken += ch;
        }
        token += content[curPos++];
    }
    if(copyToken == "main" || copyToken == "int" || copyToken == "char" || copyToken == "const" || copyToken == "if" || copyToken == "else" || copyToken == "for" || copyToken == "return" || copyToken == "break" || copyToken == "continue" || copyToken == "getchar" || copyToken == "getint" || copyToken == "printf"  || copyToken == "void") {
        return copyToken;
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
    canUse = false; //使用这个接口，就无法建立Tokens
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

    if(curPos < content.size() && content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '/') {
        // 注释
        while (curPos < content.size() && content[curPos] != '\n') {
            curPos++;
        }
        if(curPos < content.size()) {
            lineNum++;
            curPos++;
        }
        goSpacetoNext();
    }
    else if(curPos < content.size() && content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '*') {
        // 注释
        curPos += 2;
        while (curPos < content.size() && !(content[curPos] == '*' && curPos + 1 < content.size() && content[curPos + 1] == '/')) {
            if (content[curPos] == '\n') {
                lineNum++;
            }
            curPos++;
        }
        if(curPos < content.size() ) {
            curPos += 2;
        }
        goSpacetoNext();
    }
    return ;
}

bool Lexer::buildTokens() { //构建接口2
    allTokens = new std::vector<Token *>();
        if(!canUse) {
            return false;
        }
        next();
        while (curLexType != ConstType::OVERFILE) {
            Token *token = new Token(curLexType,curToken,lineNum);
            allTokens->push_back(token);
            next();
        }
        allTokens->push_back(new Token(ConstType::OVERFILE, "", -1));
        return true;

}

Token * Lexer:: nextToken() {//接口2
    index++;
    if (index < allTokens->size()) {
        return allTokens->at(index);
    }

    return new Token(ConstType::OVERFILE, "", -1);
}

Token *Lexer::getOneToken(int skew) {//接口2
    try {
        return allTokens->at(index + skew);
    }
    catch (const std::out_of_range& e) {
        return new Token(ConstType::OVERFILE, "", -1);
    }
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
