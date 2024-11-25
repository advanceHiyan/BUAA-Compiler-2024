//
// Created by aba15 on 2024/10/9.
//

#include "Parser.h"
#include "../fileIO/FileIO.h"

Parser::Parser(Lexer &lexer) : lexer(lexer) {
    currentToken = nullptr;
}

Node *Parser::parse() {
        lexer.buildTokens();
        return parseCompUnit();
}

//编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
Node *Parser::parseCompUnit() {
    try {
        Node *node = new Node(ParsingItem :: CompUnit, nullptr, nullptr);
        currentToken = lexer.nextToken();//前进一步
        while(lexer.getOneToken(1)->tokenType != ConstType::MAINTK) {
            if(lexer.getOneToken(2)->tokenType == ConstType::LPARENT) {
                node->addChild(parseFuncDef(node));
            } else {
                node->addChild(parseDecl(node));
            }
        }
        node->addChild(parseMainFuncDef(node));
        outPrintTree(node);
        return node;
    } catch (...) {
        cout << "Error: parseCompUnit" << endl;
        return  new Node(ParsingItem :: CompUnit, nullptr, nullptr);
    }
}

//声明 Decl → ConstDecl | VarDecl
Node *Parser::parseDecl(Node *parent) {
    Node *node = new Node(ParsingItem::Decl, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::CONSTTK) {
        node->addChild(parseConstDecl(node));
    } else {
        node->addChild(parseVarDecl(node));
    }
    outPrintTree(node);
    return node;
}

//常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
Node *Parser::parseConstDecl(Node *parent) {
    Node *node = new Node(ParsingItem::ConstDecl, nullptr, parent);
    node->addChild(parseOverToken(node));//CONSTTK属于终结符
    node->addChild(parseBType(node));
    node->addChild(parseConstDef(node));
    while(lexer.getOneToken()->tokenType == ConstType::COMMA) {
        node->addChild(parseOverToken(node));//COMMA属于终结符
        node->addChild(parseConstDef(node));
    }
    if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
        FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
        node->addChild(createTokenForLess(node,";"));
    }
    else
        node->addChild(parseOverToken(node));//SEMICN属于终结符
    outPrintTree(node);
    return node;
}

//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
Node *Parser::parseBType(Node *parent) {
    Node *node = new Node(ParsingItem::BType, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::INTTK) {
        node->addChild(parseOverToken(node));//INTTK属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::CHARTK) {
        node->addChild(parseOverToken(node));//CHARTK属于终结符
    }
    outPrintTree(node);
    return node;
}

//常量定义 ConstDef → Ident [ '[' ConstExp ']' ] '=' ConstInitVal // 包含普通变量、一维数组
Node *Parser::parseConstDef(Node *parent) {
    Node *node = new Node(ParsingItem::ConstDef, nullptr, parent);
    node->addChild(parseOverToken(node));//Ident属于终结符
    if(lexer.getOneToken()->tokenType == ConstType::LBRACK) {
        node->addChild(parseOverToken(node));//LBRACK属于终结符
        node->addChild(parseConstExp(node));
        if(lexer.getOneToken()->tokenType != ConstType::RBRACK) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"k");
            node->addChild(createTokenForLess(node,"]"));
        }
        else
            node->addChild(parseOverToken(node));//RBRACK属于终结符
    }
    node->addChild(parseOverToken(node));//ASSIGN属于终结符
    node->addChild(parseConstInitVal(node));
    outPrintTree(node);
    return node;
}

//常量初值 ConstInitVal → ConstExp | '{' [ ConstExp { ',' ConstExp } ] '}' |StringConst // 1.常表达式初值 2.一维数组初值
Node *Parser::parseConstInitVal(Node *parent) {
    Node *node = new Node(ParsingItem::ConstInitVal, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::STRCON) {
        node->addChild(parseOverToken(node));//STRCON属于终结符
    }
    else if(lexer.getOneToken()->tokenType == ConstType::LBRACE) {
        node->addChild(parseOverToken(node));//LBRACE属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::RBRACE) {
            node->addChild(parseConstExp(node));
            if (lexer.getOneToken()->tokenType != ConstType::RBRACE) {
                while (lexer.getOneToken()->tokenType == ConstType::COMMA) {
                    node->addChild(parseOverToken(node));//COMMA属于终结符
                    node->addChild(parseConstExp(node));
                }
            }
        }
        node->addChild(parseOverToken(node));//RBRACE属于终结符
    } else {
        node->addChild(parseConstExp(node));
    }
    outPrintTree(node);
    return node;
}

//变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
Node *Parser::parseVarDecl(Node *parent) {
    Node *node = new Node(ParsingItem::VarDecl, nullptr, parent);
    node->addChild(parseBType(node));
    node->addChild(parseVarDef(node));
    while(lexer.getOneToken()->tokenType == ConstType::COMMA) {
        node->addChild(parseOverToken(node));//COMMA属于终结符
        node->addChild(parseVarDef(node));
    }
    if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
        FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
        node->addChild(createTokenForLess(node,";"));
    }
    else
        node->addChild(parseOverToken(node));//SEMICN属于终结符
    outPrintTree(node);
    return node;
}

//变量定义 VarDef → Ident [ '[' ConstExp ']' ] | Ident [ '[' ConstExp ']' ] '='
//InitVal
Node *Parser::parseVarDef(Node *parent) {
    Node *node = new Node(ParsingItem::VarDef, nullptr, parent);
    node->addChild(parseOverToken(node));//Ident属于终结符
    if(lexer.getOneToken()->tokenType == ConstType::LBRACK) {
        node->addChild(parseOverToken(node));//LBRACK属于终结符
        node->addChild(parseConstExp(node));
        if(lexer.getOneToken()->tokenType != ConstType::RBRACK) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"k");
            node->addChild(createTokenForLess(node,"]"));
        }
        else
            node->addChild(parseOverToken(node));//RBRACK属于终结符
    }
    if(lexer.getOneToken()->tokenType == ConstType::ASSIGN) {
        node->addChild(parseOverToken(node));//ASSIGN属于终结符
        node->addChild(parseInitVal(node));
    }
    outPrintTree(node);
    return node;
}

//变量初值 InitVal → Exp | '{' [ Exp { ',' Exp } ] '}' | StringConst // 1.表达式初值 2.一维数组初值
Node *Parser::parseInitVal(Node *parent) {
    Node *node = new Node(ParsingItem::InitVal, nullptr, parent);
        if (lexer.getOneToken()->tokenType == ConstType::LBRACE) { //{
            node->addChild(parseOverToken(node));//LBRACE属于终结符
            if(lexer.getOneToken()->tokenType != ConstType::RBRACE) {
                node->addChild(parseExp(node));
                if (lexer.getOneToken()->tokenType != ConstType::RBRACE) {
                    while (lexer.getOneToken()->tokenType == ConstType::COMMA) {
                        node->addChild(parseOverToken(node));//COMMA属于终结符
                        node->addChild(parseExp(node));
                    }
                }
            }
            node->addChild(parseOverToken(node));//RBRACE属于终结符
        } else if(lexer.getOneToken()->tokenType == ConstType::STRCON){ //字符串常量
            node->addChild(parseOverToken(node));//属于终结符
        } else {
            node->addChild(parseExp(node));
        }
    outPrintTree(node);
        return node;
}


//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
Node *Parser::parseFuncDef(Node *parent) {
    Node *node = new Node(ParsingItem::FuncDef, nullptr, parent);
    node->addChild(parseFuncType(node));
    node->addChild(parseOverToken(node));//Ident属于终结符
    node->addChild(parseOverToken(node));//LPARENT属于终结符

    if(lexer.getOneToken()->tokenType== ConstType::RPARENT) {
        node->addChild(parseOverToken(node));//RPARENT属于终结符
    } else  {
        if(lexer.getOneToken() ->tokenType == ConstType::LBRACE) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
            node->addChild(createTokenForLess(node,")"));
        } else {
            node->addChild(parseFuncFParams(node));
            if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
                node->addChild(createTokenForLess(node,")"));
            }
            else
                node->addChild(parseOverToken(node));//RPARENT属于终结符
        }
    }

    node->addChild(parseBlock(node));
    outPrintTree(node);
    return node;
}

//主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block
Node *Parser::parseMainFuncDef(Node *parent) {
    Node *node = new Node(ParsingItem::MainFuncDef, nullptr, parent);
    node->addChild(parseOverToken(node));//INTTK属于终结符
    node->addChild(parseOverToken(node));//MAINTK属于终结符
    node->addChild(parseOverToken(node));//LPARENT属于终结符
    if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
        FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
        node->addChild(createTokenForLess(node,")"));
    }
    else
        node->addChild(parseOverToken(node));//RPARENT属于终结符
    node->addChild(parseBlock(node));
    outPrintTree(node);
    return node;
}

//函数类型 FuncType → 'void' | 'int' | 'char'
Node *Parser::parseFuncType(Node *parent) {
    Node *node = new Node(ParsingItem::FuncType, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::VOIDTK) {
        node->addChild(parseOverToken(node));//VOIDTK属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::INTTK) {
        node->addChild(parseOverToken(node));//INTTK属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::CHARTK) {
        node->addChild(parseOverToken(node));//CHARTK属于终结符
    }
    outPrintTree(node);
    return node;
}

//函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
Node *Parser::parseFuncFParams(Node *parent) {
    Node *node = new Node(ParsingItem::FuncFParams, nullptr, parent);
    node->addChild(parseFuncFParam(node));
    while(lexer.getOneToken()->tokenType == ConstType::COMMA) {
        node->addChild(parseOverToken(node));//COMMA属于终结符
        node->addChild(parseFuncFParam(node));
    }
    outPrintTree(node);
    return node;
}

//函数形参 FuncFParam → BType Ident ['[' ']']
Node *Parser::parseFuncFParam(Node *parent) {
    Node *node = new Node(ParsingItem::FuncFParam, nullptr, parent);
    node->addChild(parseBType(node));
    node->addChild(parseOverToken(node));//Ident属于终结符
    if(lexer.getOneToken()->tokenType == ConstType::LBRACK) {
        node->addChild(parseOverToken(node));//LBRACK属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::RBRACK) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"k");
            node->addChild(createTokenForLess(node,"]"));
        }
        else
            node->addChild(parseOverToken(node));//RBRACK属于终结符
    }
    outPrintTree(node);
    return node;
}


//语句块 Block → '{' { BlockItem } '}' /
Node *Parser::parseBlock(Node *parent) {
    Node *node = new Node(ParsingItem::Block, nullptr, parent);
    node->addChild(parseOverToken(node));//LBRACE属于终结符
    while(lexer.getOneToken()->tokenType != ConstType::RBRACE) {
        node->addChild(parseBlockItem(node));
    }
    node->addChild(parseOverToken(node));//RBRACE属于终结符
    outPrintTree(node);
    return node;
}

//语句块项 BlockItem → Decl | Stmt
Node *Parser::parseBlockItem(Node *parent) {
    Node *node = new Node(ParsingItem::BlockItem, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::CONSTTK ||
    lexer.getOneToken()->tokenType == ConstType::INTTK || lexer.getOneToken()->tokenType == ConstType::CHARTK) {
        node->addChild(parseDecl(node));
    } else {
        node->addChild(parseStmt(node));
    }
    outPrintTree(node);
    return node;
}

//语句 Stmt → LVal '=' Exp ';' // 每种类型的语句都要覆盖
//| [Exp] ';' //有无Exp两种情况
//| Block
//| 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
//| 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt // 1. 无缺省，1种情况 2.
//ForStmt与Cond中缺省一个，3种情况 3. ForStmt与Cond中缺省两个，3种情况 4. ForStmt与Cond全部
//缺省，1种情况
//| 'break' ';' | 'continue' ';'
//| 'return' [Exp] ';' // 1.有Exp 2.无Exp
//| LVal '=' 'getint''('')'';'
//| LVal '=' 'getchar''('')'';'
//| 'printf''('StringConst {','Exp}')'';' // 1.有Exp 2.无Exp
Node *Parser::parseStmt(Node *parent) {
    Node *node = new Node(ParsingItem::Stmt, nullptr, parent);
    bool flag = false;
    if(lexer.getOneToken()->tokenType == ConstType::IDENFR) {
        int line = lexer.getOneToken()->lineNumber;
        int i = 0;
        while (lexer.getOneToken(i)->lineNumber == line) {
            if(lexer.getOneToken(i)->tokenType == ConstType::ASSIGN) {
                flag = true;
            }
            i++;
        }
    }
    if(lexer.getOneToken()->tokenType == ConstType::SEMICN) {
        node->addChild(parseOverToken(node));//SEMICN属于终结符
    }
    else if (lexer.getOneToken()->tokenType == ConstType::IDENFR && flag) { // LVal '=' Exp ';'
        // || LVal '=' 'getint''('')'';' || LVal '=' 'getchar''('')'';'
        node->addChild(parseLVal(node));
        node->addChild(parseOverToken(node));//ASSIGN属于终结符
        if(lexer.getOneToken()->tokenType == ConstType::GETINTTK ||
        lexer.getOneToken()->tokenType == ConstType::GETCHARTK) {
            node->addChild(parseOverToken(node));//GETINTTK或GETCHARTK属于终结符
            node->addChild(parseOverToken(node));//LPARENT属于终结符
            if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
                node->addChild(createTokenForLess(node,")"));
            }
            else
                node->addChild(parseOverToken(node));//RPARENT属于终结符
            if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
                node->addChild(createTokenForLess(node,";"));
            }
            else
                node->addChild(parseOverToken(node));//SEMICN属于终结符
        } else {
            node->addChild(parseExp(node));
            if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
                node->addChild(createTokenForLess(node,";"));
            }
            else
                node->addChild(parseOverToken(node));//SEMICN属于终结符
        }
    }
    else if (lexer.getOneToken()->tokenType == ConstType::IFTK) {
        node->addChild(parseOverToken(node));//IFTK属于终结符
        node->addChild(parseOverToken(node));//LPARENT属于终结符
        node->addChild(parseCond(node));
        if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
            node->addChild(createTokenForLess(node,")"));
        }
        else
            node->addChild(parseOverToken(node));//RPARENT属于终结符
        node->addChild(parseStmt(node));
        if(lexer.getOneToken()->tokenType == ConstType::ELSETK) {
            node->addChild(parseOverToken(node));//ELSETK属于终结符
            node->addChild(parseStmt(node));
        }
    }
    else if (lexer.getOneToken()->tokenType == ConstType::FORTK) {
        node->addChild(parseOverToken(node));//FORTK属于终结符
        node->addChild(parseOverToken(node));//LPARENT属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            node->addChild(parseForStmt(node));
        }
        node->addChild(parseOverToken(node));//SEMICN属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            node->addChild(parseCond(node));
        }
        node->addChild(parseOverToken(node));//SEMICN属于终结符;
        if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
            node->addChild(parseForStmt(node));
            node->addChild(parseOverToken(node));//RPARENT属于终结符
        } else {
            node->addChild(parseOverToken(node));//RPARENT属于终结符
        }
        node->addChild(parseStmt(node));
    }
    else if (lexer.getOneToken()->tokenType == ConstType::BREAKTK) { //break
        node->addChild(parseOverToken(node));//BREAKTK属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
            node->addChild(createTokenForLess(node,";"));
        }
        else
            node->addChild(parseOverToken(node));//SEMICN属于终结符
    }
    else if (lexer.getOneToken()->tokenType == ConstType::CONTINUETK) { //continue
        node->addChild(parseOverToken(node));//CONTINUETK属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
            node->addChild(createTokenForLess(node,";"));
        }
        else
            node->addChild(parseOverToken(node));//SEMICN属于终结符
    }
    else if (lexer.getOneToken()->tokenType == ConstType::RETURNTK) { //return
        node->addChild(parseOverToken(node));//RETURNTK属于终结符
        if(lexer.getOneToken()->lineNumber == lexer.getOneToken(-1)->lineNumber) {
            if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
                node->addChild(parseExp(node));
            }
            if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
                node->addChild(createTokenForLess(node,";"));
            }
            else
                node->addChild(parseOverToken(node));//SEMICN属于终结符
        } else {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
            node->addChild(createTokenForLess(node,";"));
        }
    }
    else if(lexer.getOneToken()->tokenType == ConstType::PRINTFTK) { //printf
        node->addChild(parseOverToken(node));//PRINTFTK属于终结符
        node->addChild(parseOverToken(node));//LPARENT属于终结符
        node->addChild(parseOverToken(node));//STRCON属于终结符
            while(lexer.getOneToken()->tokenType == ConstType::COMMA) {
                node->addChild(parseOverToken(node));//COMMA属于终结符
                node->addChild(parseExp(node));
            }
        if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
            node->addChild(createTokenForLess(node,")"));
        }
        else
            node->addChild(parseOverToken(node));//RPARENT属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
            node->addChild(createTokenForLess(node,";"));
        }
        else
            node->addChild(parseOverToken(node));//SEMICN属于终结符
    }
    else if(lexer.getOneToken()->tokenType == ConstType::LBRACE) {
        node->addChild(parseBlock(node));
    }
    else {
        node->addChild(parseExp(node));
        if(lexer.getOneToken()->tokenType != ConstType::SEMICN) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"i");
            node->addChild(createTokenForLess(node,";"));
        }
        else
            node->addChild(parseOverToken(node));//SEMICN属于终结符
    }
    outPrintTree(node);
    return node;
}

//语句 ForStmt → LVal '=' Exp
Node *Parser::parseForStmt(Node *parent) {
    Node *node = new Node(ParsingItem::ForStmt, nullptr, parent);
    node->addChild(parseLVal(node));
    node->addChild(parseOverToken(node));//ASSIGN属于终结符
    node->addChild(parseExp(node));
    outPrintTree(node);
    return node;
}

//表达式 Exp → AddExp // 存在即可
Node *Parser::parseExp(Node *parent) {
    Node *node = new Node(ParsingItem::Exp, nullptr, parent);
    node->addChild(parseAddExp(node));
    outPrintTree(node);
    return node;
}

//条件表达式 Cond → LOrExp
Node *Parser::parseCond(Node *parent) {
    Node *node = new Node(ParsingItem::Cond, nullptr, parent);
    node->addChild(parseLOrExp(node));
    outPrintTree(node);
    return node;
}

//基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number | Character
Node *Parser::parsePrimaryExp(Node *parent) {
    Node *node = new Node(ParsingItem::PrimaryExp, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::LPARENT) {
        node->addChild(parseOverToken(node));//LPARENT属于终结符
        node->addChild(parseExp(node));
        if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
            node->addChild(createTokenForLess(node,")"));
        }
        else
            node->addChild(parseOverToken(node));//RPARENT属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::IDENFR) {
        node->addChild(parseLVal(node));
    } else if(lexer.getOneToken()->tokenType == ConstType::INTCON) {
        node->addChild(parseNumber(node));//NUMCON属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::CHRCON) {
        node->addChild(parseCharactor(node));//CHARTK不属于终结符
    } else {

    }
    outPrintTree(node);
    return node;
}

//数值 Number → IntConst
Node *Parser::parseNumber(Node *parent) {
    Node *node = new Node(ParsingItem::Number, nullptr, parent);
    node->addChild(parseOverToken(node));//NUMCON属于终结符
    outPrintTree(node);
    return node;
}
//字符 Character → CharConst
Node *Parser::parseCharactor(Node *parent) {
    Node *node = new Node(ParsingItem::Character, nullptr, parent);
    node->addChild(parseOverToken(node));//CHARTK属于终结符
    outPrintTree(node);
    return node;
}

//一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
Node *Parser::parseUnaryExp(Node *parent) {
    Node *node = new Node(ParsingItem::UnaryExp, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::IDENFR &&
    lexer.getOneToken(1)->tokenType == ConstType::LPARENT) {
        node->addChild(parseOverToken(node));//Ident属于终结符
        node->addChild(parseOverToken(node));//LPARENT属于终结符
        if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
            if (lexer.getOneToken()->tokenType == ConstType::SEMICN ||
            lexer.getOneToken() ->tokenType == ConstType::ASSIGN) {
                FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
                node->addChild(createTokenForLess(node,")"));
            }
            else {
                node->addChild(parseFuncRParams(node));
                if(lexer.getOneToken()->tokenType != ConstType::RPARENT) {
                    FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"j");
                    node->addChild(createTokenForLess(node,")"));
                }
                else
                    node->addChild(parseOverToken(node));//RPARENT属于终结符
            }
        } else {
            node->addChild(parseOverToken(node));//RPARENT属于终结符
        }
    } else if(lexer.getOneToken()->tokenType == ConstType::PLUS ||
    lexer.getOneToken()->tokenType == ConstType::MINU ||
    lexer.getOneToken()->tokenType == ConstType::NOT) {
        node->addChild(parseUnaryOp(node));
        node->addChild(parseUnaryExp(node));
    } else {
        node->addChild(parsePrimaryExp(node));
    }
    outPrintTree(node);
    return node;
}

//单目运算符 UnaryOp → '+' | '−' | '!'
Node *Parser::parseUnaryOp(Node *parent) {
    Node *node = new Node(ParsingItem::UnaryOp, nullptr, parent);
    if(lexer.getOneToken()->tokenType == ConstType::PLUS) {
        node->addChild(parseOverToken(node));//PLUS属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::MINU) {
        node->addChild(parseOverToken(node));//MINU属于终结符
    } else if(lexer.getOneToken()->tokenType == ConstType::NOT) {
        node->addChild(parseOverToken(node));//NOT属于终结符
    }
    outPrintTree(node);
    return node;
}

//函数实参表 FuncRParams → Exp { ',' Exp }
Node *Parser::parseFuncRParams(Node *parent) {
    Node *node = new Node(ParsingItem::FuncRParams, nullptr, parent);
    node->addChild(parseExp(node));
    while(lexer.getOneToken()->tokenType == ConstType::COMMA) {
        node->addChild(parseOverToken(node));//COMMA属于终结符
        node->addChild(parseExp(node));
    }
    outPrintTree(node);
    return node;
}

//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
Node *Parser::parseMulExp(Node *parent) {
    Node *node = new Node(ParsingItem::MulExp, nullptr, parent);
    node->addChild(parseUnaryExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::MULT ||
    lexer.getOneToken()->tokenType == ConstType::DIV ||
    lexer.getOneToken()->tokenType == ConstType::MOD) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::MulExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //Mul -> (Mul */% UnaryExp)
        node->addChild(parseOverToken(node));//MUL或DIV或MOD属于终结符
        node->addChild(parseUnaryExp(node));
    }
    outPrintTree(node);
    return node;
}

//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
Node *Parser::parseAddExp(Node *parent) {
    Node *node = new Node(ParsingItem::AddExp, nullptr, parent);
    node->addChild(parseMulExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::PLUS ||
    lexer.getOneToken()->tokenType == ConstType::MINU) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::AddExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //Add -> (Add +/- MulExp)
        node->addChild(parseOverToken(node));//PLUS或MINU属于终结符
        node->addChild(parseMulExp(node));
    }
    outPrintTree(node);
    return node;
}

//相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp
Node *Parser::parseEqExp(Node *parent) {
    Node *node = new Node(ParsingItem::EqExp, nullptr, parent);
    node->addChild(parseRelExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::EQL ||
    lexer.getOneToken()->tokenType == ConstType::NEQ) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::EqExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //Eq -> (Eq ==!= RelExp)
        node->addChild(parseOverToken(node));//EQL或NEQ属于终结符
        node->addChild(parseRelExp(node));
    }
    outPrintTree(node);
    return node;
}

//关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
Node *Parser::parseRelExp(Node *parent) {
    Node *node = new Node(ParsingItem::RelExp, nullptr, parent);
    node->addChild(parseAddExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::LSS ||
    lexer.getOneToken()->tokenType == ConstType::GRE ||
    lexer.getOneToken()->tokenType == ConstType::LEQ ||
    lexer.getOneToken()->tokenType == ConstType::GEQ) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::RelExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //Rel -> (Rel < > <= >= AddExp)
        node->addChild(parseOverToken(node));//LSS或GRE或LEQ或GEQ属于终结符
        node->addChild(parseAddExp(node));
    }
    outPrintTree(node);
    return node;
}



//左值表达式 LVal → Ident ['[' Exp ']']
Node *Parser::parseLVal(Node *parent) {
    Node *node = new Node(ParsingItem::LVal, nullptr, parent);
    node->addChild(parseOverToken(node));//Ident属于终结符
    if(lexer.getOneToken()->tokenType == ConstType::LBRACK) {
        node->addChild(parseOverToken(node));//LBRACK属于终结符
        node->addChild(parseExp(node));
        if(lexer.getOneToken()->tokenType != ConstType::RBRACK) {
            FileIO::printToFile_Error(lexer.getOneToken(-1)->lineNumber,"k");
            node->addChild(createTokenForLess(node,"]"));
        }
        else
            node->addChild(parseOverToken(node));//RBRACK属于终结符
    }
    outPrintTree(node);
    return node;
}

//常量表达式 ConstExp → AddExp
Node *Parser::parseConstExp(Node *parent) {
    Node *node = new Node(ParsingItem::ConstExp, nullptr, parent);
    node->addChild(parseAddExp(node));
    outPrintTree(node);
    return node;
}

//逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
Node *Parser::parseLAndExp(Node *parent) {
    Node *node = new Node(ParsingItem::LAndExp, nullptr, parent);
    node->addChild(parseEqExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::AND ||
            lexer.getOneToken()->tokenType == ConstType::SINGLEAND) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::LAndExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //LAnd -> (LAnd && EqExp)
        node->addChild(parseOverToken(node));//AND或SINGLEAND属于终结符
        node->addChild(parseEqExp(node));
    }
    outPrintTree(node);
    return node;
}

//逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
Node *Parser::parseLOrExp(Node *parent) {
    Node *node = new Node(ParsingItem::LOrExp, nullptr, parent);
    node->addChild(parseLAndExp(node));

    while(lexer.getOneToken()->tokenType == ConstType::OR ||
            lexer.getOneToken()->tokenType == ConstType::SINGLEOR) {
        //build parent and newNode don't need to add child to parent
        Node * newNode = new Node(ParsingItem::LOrExp, nullptr, parent);
        //remove node and parent don't need to remove child from parent
        node->parent = newNode;
        //build newNode and node
        newNode->addChild(node);
        //print node
        outPrintTree(node);
        //change newNode to node
        node = newNode;
        newNode = nullptr;
        //LOr -> (LOr || LAndExp)
        node->addChild(parseOverToken(node));//OR或SINGLEOR属于终结符
        node->addChild(parseLAndExp(node));
    }
    outPrintTree(node);
    return node;
}

Node *Parser::parseOverToken(Node *parent) {
    Node *node = new Node(ParsingItem::OverToken, currentToken, parent);
    FileIO::printToFile_Lexer(*currentToken); //输出当前token
    currentToken = lexer.nextToken(); //前进一步
    return node;
}

Node * Parser :: createTokenForLess(Node *parent,std::string str) {
    Token *token = new Token(ConstType::CreateForLessToken,str,-1);
    Node *node = new Node(ParsingItem::OverToken, token, parent);
    return node;
}

void Parser::outPrintTree(Node(*node)) {
    if(node->notOutput()) {
        return;
    }
    FileIO::printToFile_Grammar(node->parsingItem);
}


