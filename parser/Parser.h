// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

//
// Created by aba15 on 2024/10/9.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H


#include "Node.h"
#include "../lexer/Lexer.h"
#include "../fileIO/ConstType.h"
class Parser {
    Token *currentToken;
    Lexer &lexer;
    public:
    Parser(Lexer &lexer);
    Node *parse();
    Node *parseCompUnit();
    Node *parseMainFuncDef(Node *parent);
    Node *parseConstDecl(Node *parent);
    Node *parseConstDef(Node *parent);
    Node *parseConstInitVal(Node *parent);
    Node *parseVarDecl(Node *parent);
    Node *parseVarDef(Node *parent);
    Node *parseInitVal(Node *parent);
    Node *parseFuncDef(Node *parent);
    Node *parseFuncType(Node *parent);
    Node *parseFuncFParams(Node *parent);
    Node *parseFuncFParam(Node *parent);
    Node *parseBlock(Node *parent);
    Node *parseStmt(Node *parent);
    Node *parseForStmt(Node *parent);
    Node *parseExp(Node *parent);
    Node *parseCond(Node *parent);
    Node *parseLVal(Node *parent);
    Node *parsePrimaryExp(Node *parent);
    Node *parseUnaryExp(Node *parent);
    Node *parseUnaryOp(Node *parent);
    Node *parseFuncRParams(Node *parent);
    Node *parseMulExp(Node *parent);
    Node *parseAddExp(Node *parent);
    Node *parseRelExp(Node *parent);
    Node *parseEqExp(Node *parent);
    Node *parseLAndExp(Node *parent);
    Node *parseLOrExp(Node *parent);
    Node *parseConstExp(Node *parent);
    Node *parseBlockItem(Node *parent);
    Node *parseBType(Node *parent);
    Node *parseDecl(Node *parent);

    Node *parseNumber(Node *parent);//是编译单元
    Node *parseCharactor(Node *parent);//是编译单元

    Node *parseOverToken(Node *parent); //终结符，包括Ident等等

    void outPrintTree(Node *node);

    Node *createTokenForLess(Node *parent, std::string str);
};


#endif //COMPILER_PARSER_H
