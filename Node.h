//
// Created by aba15 on 2024/10/9.
//

#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include <iostream>
#include <vector>
#include "ConstType.h"
struct Token {
    ConstType tokenType;
    std::string tokenValue;
    int lineNumber;

    Token (ConstType tokenType, std::string tokenValue, int lineNumber)
            : tokenType(tokenType), tokenValue(tokenValue), lineNumber(lineNumber) {}
};

struct Node {
    ParsingItem parsingItem;
    Token *token; // 终结符

    Node *parent;
    int depth;
    std::vector<Node *> children;

    Node(ParsingItem parsingItem, Token *token, Node *parent, int depth) {
        this->parsingItem = parsingItem;
        this->token = token;
        this->parent = parent;
        this->depth = depth;
    }

    Node(ParsingItem parsingItem, Node *parent, int depth) {
        this->parsingItem = parsingItem;
        this->token = nullptr;
        this->parent = parent;
        this->depth = depth;
    }

    Node(ParsingItem parsingItem, Node *parent) {
        this->parsingItem = parsingItem;
        this->token = nullptr;
        this->parent = parent;
        this->depth = 0;
    }

    Node(ParsingItem parsingItem, Token *token, Node *parent) {
        this->parsingItem = parsingItem;
        this->token = token;
        this->parent = parent;
        this->depth = 0;
    }

    void addChild(Node *child) {
        if(child!= nullptr) {
            return;
        }
        this->children.push_back(child);
    }

    bool notOutput() const {
        return ( this ->token != nullptr || //终结符
            this ->parsingItem == ParsingItem :: BlockItem ||
            this ->parsingItem == ParsingItem :: BType ||
            this ->parsingItem == ParsingItem :: Decl);
    }
};

#endif //COMPILER_NODE_H
