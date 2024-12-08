// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

//
// Created by aba15 on 2024/11/22.
//

#include "ToPcodeVisitor.h"
using namespace std;
int IfLabel::label_num = 0;
int ForLabel::label_num = 0;
int TempLabel::label_num = 0;
//编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block // 存在main函数
void ToPcodeVisitor::visit(Node *node) {
    overall_table = new OverallSymbolTable("global", nullptr,
                                           block_num++,TableType::OverallBlock);
    //除了初始可以用迭代器遍历，其它必须用size！
    for (auto child : node->children) {
        if (child ->parsingItem == ParsingItem::Decl) {
            cout << "Decl" << endl;
            visit_Decl(child, overall_table);
        } else if(child ->parsingItem == ParsingItem::FuncDef){
            cout << "FuncDef" << endl;
            visit_FuncDef(child);
        } else {// MainFuncDef 函数名不算全局Symbol
            cout << "MainFuncDef" << endl;
            SymbolTable *main_table = new SymbolTable("main", overall_table,
                                                      block_num++,TableType::IntFunc);
            addCode(CodeType::MAIN);
            int i = child->children.size() - 1;
            visit_Block(child->children.at(i), main_table,nullptr);
            addCode(CodeType::EXIT);
        }
    }
    this -> printPcode();
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//函数类型 FuncType → 'void' | 'int' | 'char'// 覆盖三种类型的函数
void ToPcodeVisitor::visit_FuncDef(Node *func_def) {
    Token *token = func_def->children.at(1)->token;
    std::string func_name = token->tokenValue;
    std::string void_int_char_str = func_def->children.at(0)->children.at(0)->token->tokenValue;
    SymbolType func_type = void_int_char_str =="void"? SymbolType::VoidFunc :
            (void_int_char_str =="int"? SymbolType::IntFunc : SymbolType::CharFunc);
    TableType tableType = void_int_char_str =="void"? TableType::VoidFunc :
            (void_int_char_str =="int"? TableType::IntFunc : TableType::CharFunc);
    Symbol *func_symbol = new Symbol(func_name, func_type, overall_table->BlockNum);
    //函数名不添加到全局符号表
    FunSymbolTable *func_table = new FunSymbolTable(func_name, overall_table,
                                                    block_num++,tableType);
    overall_table->addFunTable(func_name,func_symbol,func_table);
    visit_FParamsAndBlock(func_def, func_table);
    addCode(CodeType::EXF);
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//FuncFParams → FuncFParam { ',' FuncFParam }
// FuncFParam → BType Ident ['[' ']']
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
void ToPcodeVisitor::visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table) {
    std::string func_name = func_def->children.at(1)->token->tokenValue;
    bool have_params = false;
    for (int i = 2; i < func_def->children.size(); i++) {
        if (func_def->children.at(i)->parsingItem == ParsingItem::FuncFParams) {
            Node *fparams = func_def->children.at(i);

            int size = fparams->children.size() / 2 + 1;
            addCode(CodeType::FUNC,func_name,size);
            have_params = true;

            for(int j = 0; j < fparams->children.size(); j++) {
                if(fparams->children.at(j)->parsingItem == ParsingItem::FuncFParam) {
                    Node *fparam = fparams->children.at(j);
                    std::string  char_or_int = fparam->children.at(0)->children.at(0)->token->tokenValue;
                    std::string name = fparam->children.at(1)->token->tokenValue;
                    if(fparam->children.size() == 2) {
                        SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
                        this_table->paramTypes.push_back(type);

                        CodeType codeType = type == SymbolType::Int? CodeType::PARINT : CodeType::PARCHAR;
                        addCode(codeType,std::to_string(this_table -> BlockNum) + "_" + name);

                        this_table->addSymbol(symbol->name,symbol);
                    } else {
                        SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);

                        CodeType codeType = type == SymbolType::IntArray ? CodeType::PARINTARRAY : CodeType::PARCHARARRAY;
                        addCode(codeType,std::to_string(this_table -> BlockNum) + "_" + name);

                        this_table->paramTypes.push_back(type);
                        this_table->addSymbol(symbol->name,symbol);
                    }
                }
            }
        } else if (func_def->children.at(i)->parsingItem == ParsingItem::Block) {

            if(!have_params) {
                addCode(CodeType::FUNC,func_name,0);
            }

            visit_Block(func_def->children.at(i), this_table,nullptr);
        }
    }
}

//语句块 Block → '{' { BlockItem } '}'
//语句块项 BlockItem → Decl | Stmt
void ToPcodeVisitor::visit_Block(Node *block, SymbolTable *this_table,ForLabel *forLabel) {
    for(int i = 1; i < block->children.size()-1; i++) {
        if(block->children.at(i)->parsingItem == ParsingItem::BlockItem) {
            Node *decl_or_stmt = block->children.at(i) ->children.at(0);
            if(decl_or_stmt->parsingItem == ParsingItem::Decl) {
                visit_Decl(decl_or_stmt, this_table);
            } else {
                visit_Stmt(decl_or_stmt, this_table,forLabel);
            }
        }
    }
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
void ToPcodeVisitor::visit_Stmt(Node *stmt, SymbolTable *this_table,ForLabel *forLabel) {
    if(stmt->children.size() == 1 && stmt->children.at(0)->parsingItem == ParsingItem::OverToken) {//;
        return;//空语句
    } else if(stmt->children.at(0)->parsingItem == ParsingItem::Block) {//if{}else{} 空{}
        SymbolTable *new_table = new SymbolTable("emptyTable", this_table,
                                                 block_num++,TableType::EmptyBlock);
        visit_Block(stmt->children.at(0), new_table,forLabel);
    } else if (stmt->children.at(0)->parsingItem == ParsingItem::LVal) {
        Symbol *symbol = this_table->getSymbol(stmt->children.at(0)->children.at(0)->token->tokenValue);
        if(stmt->children.at(2) ->parsingItem == ParsingItem::Exp) {//LVal = Exp;
            visit_LVal(stmt->children.at(0), this_table);
            addCode(CodeType::LDA,std::to_string(symbol->blockNum) + "_" + symbol->name,-1);
            visit_Exp_or_ConstExp(stmt->children.at(2), this_table);
        } else {//LVal = getint();LVal = getchar();
            visit_LVal(stmt->children.at(0), this_table);
            addCode(CodeType::LDA,std::to_string(symbol->blockNum) + "_" + symbol->name,-1);
            if (stmt -> children.at(2) -> token -> tokenValue == "getint") {
                addCode(CodeType::GETINT);
            } else {
                addCode(CodeType::GETCHAR);
            }
        }
        addCode(CodeType::PAL);
    } else if(stmt ->children.at(0) ->parsingItem == ParsingItem::Exp) {//exp;
        visit_Exp_or_ConstExp(stmt->children.at(0), this_table);
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "if") {
        visit_If_Stmt(stmt, this_table,forLabel);
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "for") {
        visit_For_Stmt(stmt, this_table);
    } else if(stmt->children.at(0)->token != nullptr && (stmt->children.at(0)->token->
            tokenValue == "continue" ||stmt->children.at(0)->token->tokenValue == "break")) {
        if(stmt->children.at(0)->token->tokenValue == "continue") {
            addCode(CodeType::JMP,forLabel -> for_last_stmt_label);
        } else {
            addCode(CodeType::JMP,forLabel -> for_end_label);
        }
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "return") {
        if(stmt->children.size() > 2) {
            visit_Exp_or_ConstExp(stmt->children.at(1), this_table);
        }
        int isReturn = this_table -> isReturnFun();//1//2void//-1no
        addCode(CodeType::RET,isReturn);
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "printf") {
        int size = 0;
        for(int i = 2; i < stmt->children.size(); i++) {
            if(stmt->children.at(i)->parsingItem == ParsingItem::Exp) {
                visit_Exp_or_ConstExp(stmt->children.at(i), this_table);
                size += 1;
            }
        }
        addCode(CodeType::PRINTF,stmt->children.at(2)->token->tokenValue,size);
    }
    else {
        cout << "Stmt ???" << endl;
    }
}

//| 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt // 1. 无缺省，1种情况 2.
// ForStmt → LVal '=' Exp
void ToPcodeVisitor::visit_For_Stmt(Node *stmt, SymbolTable *this_table) {
    cout << "For_Stmt" << endl;
    int i = 2;
    int assign_num = 0;
    ForLabel *forLabel = new ForLabel();
    addCode(CodeType::LABEL,forLabel -> for_start_label);
    while(i < stmt ->children.size()) {
        if(stmt->children.at(i)->parsingItem == ParsingItem::ForStmt) {
            if (assign_num == 0) { // 第一个forstmt
                Node *lval = stmt->children.at(i) ->children.at(0);
                Symbol *symbol = this_table->getSymbol(lval->children.at(0)->token->tokenValue);
                visit_LVal(lval, this_table);
                addCode(CodeType::LDA,std::to_string(symbol->blockNum) + "_" + symbol->name,-1);
                visit_Exp_or_ConstExp(stmt->children.at(i) ->children.at(2), this_table);
                addCode(CodeType::PAL);
            }
        } else if (stmt->children.at(i)->parsingItem == ParsingItem::Stmt) {
            addCode(CodeType::LABEL,forLabel->for_stmt_label);
            if(stmt->children.at(i)->children.at(0) ->parsingItem == ParsingItem::Block) {
                SymbolTable *new_table = new SymbolTable("forTable", this_table,
                                                         block_num++,TableType::ForBlock);
                visit_Block(stmt->children.at(i)->children.at(0), new_table,forLabel);
            } else {
                visit_Stmt(stmt->children.at(i), this_table,forLabel);
            }
            addCode(CodeType::LABEL,forLabel -> for_last_stmt_label);
            if(stmt->children.at(i - 2) ->parsingItem == ParsingItem::ForStmt) {//第二个forstmt
                Node *lval = stmt->children.at(i - 2) ->children.at(0);
                Symbol *symbol = this_table->getSymbol(lval->children.at(0)->token->tokenValue);
                visit_LVal(lval, this_table);
                addCode(CodeType::LDA,std::to_string(symbol->blockNum) + "_" + symbol->name,-1);
                visit_Exp_or_ConstExp(stmt->children.at(i -  2) ->children.at(2), this_table);
                addCode(CodeType::PAL);
            }
        }  else if(stmt ->children.at(i) ->parsingItem == ParsingItem::OverToken) {
            if(stmt->children.at(i)->token->tokenValue == ";") {
                assign_num += 1;
                if(assign_num == 1 ) {//下一个可能是cond
                    if(stmt->children.at(i + 1)->parsingItem == ParsingItem::Cond) {
                        addCode(CodeType::LABEL,forLabel -> for_cond_label);
                        visit_Cond(stmt->children.at(i + 1), this_table,forLabel, nullptr);
                        addCode(CodeType::JPC,forLabel -> for_end_label);
                    } else {
                        addCode(CodeType::LABEL,forLabel -> for_cond_label);
                    }
                }
            }
        }
        i += 1;
    }
    addCode(CodeType::JMP,forLabel -> for_cond_label);
    addCode(CodeType::LABEL,forLabel -> for_end_label);
}

//左值表达式 LVal → Ident ['[' Exp ']']
void ToPcodeVisitor::visit_LVal(Node *lval, SymbolTable *this_table) {
    Node *ident = lval->children.at(0);
    std::string name = ident->token->tokenValue;

    if(lval->children.size() > 1) {
        visit_Exp_or_ConstExp(lval->children.at(2), this_table);
    }
}
//ConstInitVal → ConstExp | '{' [ ConstExp { ',' ConstExp } ] '}' |StringConst
void ToPcodeVisitor::visit_InitVal(Node *init_val, SymbolTable *this_table,std::string name) {
    cout << "InitVal" << endl;
    int exp_size = 0;
    for(int i = 0; i < init_val->children.size(); i++) {
        if(init_val->children.at(i)->parsingItem == ParsingItem::ConstExp ||
           init_val->children.at(i)->parsingItem == ParsingItem ::Exp ) {
            visit_Exp_or_ConstExp(init_val->children.at(i), this_table);
            exp_size += 1;
        }
    }
    if(exp_size >= 1) {
        addCode(CodeType::EMPTY,std::to_string(this_table->BlockNum) + "_" + name, exp_size);
    } else {
        string strConst = init_val->children.at(0)->token->tokenValue;
        string strPtr = strConst.substr(1, strConst.size() - 2);
        int size = 0;
        for(int i = 0; i < strPtr.size(); i++) {
            //不能用tostring，因为char是int存储。
            if(strPtr.at(i) == '\\' && i < strPtr.size() - 1) {
                char c = strPtr.at(i + 1);
                if (c == 'n') {
                    addCode(CodeType::LDC,  "\n");
                } else if (c == 't') {
                    addCode(CodeType::LDC, "\t");
                } else if (c == 'a') {
                    addCode(CodeType::LDC, "\a");
                } else if(c == 'b') {
                    addCode(CodeType::LDC,  "\b");
                } else if(c == 'f') {
                    addCode(CodeType::LDC,  "\f");
                } else if(c == '0') {
                    addCode(CodeType::LDC, +"\0");
                } else if(c == 'v') {
                    addCode(CodeType::LDC,"\v");
                } else {
                    addCode(CodeType::LDC, strPtr.substr(i + 1,1));
                }
                i++;
            } else {
                //不能用tostring，因为char是int存储。不能用at()，因为char是int存储。
                addCode(CodeType::LDC, (strPtr.substr(i,1)));
            }
            size += 1;
        }
        addCode(CodeType::EMPTY,std::to_string(this_table->BlockNum) + "_" + name, size);
    }
}

//| 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
void ToPcodeVisitor::visit_If_Stmt(Node *stmt, SymbolTable *this_table,ForLabel *forLabel) {
    IfLabel *ifLabel = new IfLabel();
    addCode(CodeType::LABEL,ifLabel->if_start_label);
    visit_Cond(stmt->children.at(2), this_table, nullptr,ifLabel);
    addCode(CodeType::JPC,ifLabel->else_label);//栈顶是0就跳转到else
    addCode(CodeType::LABEL,ifLabel->if_stmt_label);
    visit_Stmt(stmt->children.at(4), this_table,forLabel);
    addCode(CodeType::JMP,ifLabel->if_end_label);//无条件跳转到结束
    addCode(CodeType::LABEL,ifLabel->else_label);
    if(stmt->children.size() > 5) {
        visit_Stmt(stmt->children.at(6), this_table,forLabel);
    }
    addCode(CodeType::LABEL,ifLabel->if_end_label);
}

//声明 Decl → ConstDecl | VarDecl
//常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
//变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
void ToPcodeVisitor::visit_Decl(Node *node, SymbolTable *this_table) {
    if (node->children.at(0) ->parsingItem == ParsingItem::ConstDecl) {
        Node *const_decl = node->children.at(0);
        std::string  char_or_int = const_decl->children.at(1)->children.at(0)->token->tokenValue;
        for (auto const_def : const_decl->children) {
            if(const_def -> parsingItem == ParsingItem::ConstDef) {
                visit_def(const_def, this_table,char_or_int);
            }
        }
    } else {
        Node *var_decl = node->children.at(0);
        std::string  char_or_int = var_decl->children.at(0)->children.at(0)->token->tokenValue;
        for (auto var_def : var_decl->children) {
            if (var_def->parsingItem == ParsingItem::VarDef) {
                visit_def(var_def, this_table,char_or_int);
            }
        }
    }
}

//常量定义 ConstDef → Ident [ '[' ConstExp ']' ] '=' ConstInitVal
//变量定义 VarDef → Ident [ '[' ConstExp ']' ] | Ident [ '[' ConstExp ']' ] '='InitVal // 包含普通常量、一维数组定义
void ToPcodeVisitor::visit_def(Node *node, SymbolTable *this_table,std::string char_or_int) {
    cout << "def" << endl;
    std::string name = node ->children.at(0) ->token->tokenValue;
    if (true) {//不区分常量变量
        if(node->children.size() > 2 && node->children.at(2)->parsingItem == ParsingItem::ConstExp) {//数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);

            CodeType codeType = char_or_int == "int"  ? CodeType::VARINTARRAY : CodeType ::VARCHARARRAY;
            addCode(codeType,std::to_string(this_table->BlockNum) + "_" + name);

            this_table->addSymbol(symbol->name,symbol);
            visit_Exp_or_ConstExp(node->children.at(2), this_table);//!!!!!!
        } else {//不是数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);

            CodeType codeType = char_or_int == "int"  ? CodeType::VARINT : CodeType ::VARCHAR;
            addCode(codeType,std::to_string(this_table->BlockNum) + "_" + name);

            this_table->addSymbol(symbol->name,symbol);
        }
        if(node->children.at(node->children.size()-1) ->parsingItem == ParsingItem::InitVal ||
           node->children.at(node->children.size()-1) ->parsingItem == ParsingItem::ConstInitVal) {
            visit_InitVal(node->children.at(node->children.size()-1), this_table,name);
        } //!!!!!
        else {
            addCode(CodeType::EMPTY,std::to_string(this_table->BlockNum) + "_" + name, 0);
        }
    }
}

//表达式 Exp → AddExp // 存在即可

void ToPcodeVisitor::visit_Exp_or_ConstExp(Node *exp, SymbolTable *this_table) {
    cout << "Exp_or_ConstExp" << endl;
    Node *add_exp = exp->children.at(0);
    visit_AddExp(add_exp, this_table);
}

//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
void ToPcodeVisitor::visit_AddExp(Node *add_exp, SymbolTable *this_table) {
    cout << "AddExp" << endl;
    for (int i = 0; i < add_exp->children.size(); ++i) {
        Node *child = add_exp->children.at(i);
        if(child->parsingItem == ParsingItem::MulExp) {
            visit_MulExp(child, this_table);
            if(i - 1 >= 0 && add_exp->children.at(i-1)->parsingItem == ParsingItem::OverToken) {
                if(add_exp->children.at(i-1)->token->tokenValue == "+") {
                    addCode(CodeType::ADD);
                } else if(add_exp->children.at(i-1)->token->tokenValue == "-") {
                    addCode(CodeType::SUB);
                }
            }
        } else if(child -> parsingItem == ParsingItem::AddExp) {
            visit_AddExp(child, this_table);
        }
    }
//    for (Node *child : add_exp->children) {
//        if(child->parsingItem == ParsingItem::MulExp) {
//            visit_MulExp(child,this_table);
//        } else if(child -> parsingItem == ParsingItem::AddExp) {
//            visit_AddExp(child,this_table);
//        } else if(child -> parsingItem == ParsingItem::OverToken) {
//            if(child->token->tokenValue == "+") {
//                addCode(CodeType::ADD);
//            } else if(child->token->tokenValue == "-") {
//                addCode(CodeType::SUB);
//            }
//        }
//    }
}

//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
void ToPcodeVisitor ::visit_MulExp(Node * mulexp,SymbolTable * this_table) {
    cout << "MulExp" << endl;
    for (int i = 0; i < mulexp->children.size(); ++i) {
        Node *child = mulexp->children.at(i);
        if(child->parsingItem == ParsingItem::UnaryExp) {
            visit_Unary(child, this_table);
            if(i - 1 >= 0 && mulexp->children.at(i-1)->parsingItem == ParsingItem::OverToken) {
                if(mulexp->children.at(i-1)->token->tokenValue == "*") {
                    addCode(CodeType::MUL);
                } else if(mulexp->children.at(i-1)->token->tokenValue == "/") {
                    addCode(CodeType::DIV);
                } else if(mulexp->children.at(i-1)->token->tokenValue == "%") {
                    addCode(CodeType::MOD);
                }
            }
        } else if(child -> parsingItem == ParsingItem::MulExp) {
            visit_MulExp(child, this_table);
        }
    }
//    for(Node * child : mulexp ->children) {
//        if (child->parsingItem == ParsingItem::MulExp) {
//            visit_MulExp(child,this_table);
//        } else if(child -> parsingItem == ParsingItem::UnaryExp) {
//            visit_Unary(child,this_table);
//        } else if(child -> parsingItem == ParsingItem::OverToken) {
//            if(child->token->tokenValue == "*") {
//                addCode(CodeType::MUL);
//            } else if(child->token->tokenValue == "/") {
//                addCode(CodeType::DIV);
//            } else if(child->token->tokenValue == "%") {
//                addCode(CodeType::MOD);
//            }
//        }
//    }
}

//UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
//FuncRParams → Exp { ',' Exp }
void ToPcodeVisitor::visit_Unary(Node * unary,SymbolTable * this_table) {
    cout << "Unary" << endl;
    if(unary->children.at(0)->parsingItem == ParsingItem::OverToken) {//Ident
        cout << "Unary Ident" << endl;
        Node * ident = unary->children.at(0);
        std :: vector<SymbolType> * funcParams = overall_table ->findFunType(ident->token->tokenValue);
        cout << "funcParams size " << funcParams->size() << endl;
        if(funcParams != nullptr && funcParams->size() > 0) {
            Node *fun_ams = unary->children.at(2);
            int i = 0;
            for(Node * child : fun_ams->children) {
                if (child->parsingItem == ParsingItem::Exp) {
                    visit_Exp_or_ConstExp(child, this_table);
                    if(funcParams->at(i) == SymbolType::Int ||funcParams->at(i) == SymbolType::Char) {
                        addCode(CodeType::APR);
                    } else if(funcParams->at(i) == SymbolType::IntArray || funcParams->at(i) == SymbolType::CharArray) {
                        addCode(CodeType::APA);
                    } else {
                        cout << "error in code prams";
                    }
                    i++;
                }
            }
        }
        addCode(CodeType::CAL,ident->token->tokenValue);
    }
    for(int i = 0;i < unary ->children.size() ; i ++ ) {
        Node * child = unary->children.at(i);
        if (child->parsingItem == ParsingItem::PrimaryExp) {
            visit_PrimaryExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::UnaryExp) {
            cout << "UnaryExp" << endl;
            visit_Unary(child, this_table);
            if(unary->children.at(i -1)->parsingItem == ParsingItem::UnaryOp) {
                cout << "UnaryOp" << endl;
                if(unary->children.at(i -1)->children.at(0)->token->tokenValue == "-") {
                    addCode(CodeType::MUS);
                } else if(unary->children.at(i -1)->children.at(0)->token->tokenValue == "!") {
                    addCode(CodeType::NOT);
                }
            }
        }
//        else if (child -> parsingItem == ParsingItem::UnaryOp) {
//            Node * op = child;
//             if (op -> children.at(0) -> token -> tokenValue == "-") {
//                addCode(CodeType::MINU);
//            } else if (op -> children.at(0) -> token -> tokenValue == "!") {
//                addCode(CodeType::NOT);
//            }
//        }
    }
}
//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
//UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
//FuncRParams → Exp { ',' Exp }
//基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number | Character
//左值表达式 LVal → Ident ['[' Exp ']']


int ToPcodeVisitor::getExpFunType(Node *exp, SymbolTable *this_table) {
    Node *add_exp = exp->children.at(0);
    Node *mul_exp = add_exp->children.at(add_exp->children.size() - 1);
    Node *uy = mul_exp -> children.at(mul_exp->children.size() - 1);
    int used_par = -1;
    if(uy -> children.at(0) ->parsingItem == ParsingItem::OverToken) {//变量
        used_par = 0;
    } else if(uy ->children.at(0) ->parsingItem == ParsingItem::PrimaryExp) {
        Node * prim = uy ->children.at(0);
        if(prim -> children.at(0) ->parsingItem == ParsingItem::LVal) {
            Node * lv = prim -> children.at(0);
            used_par = this_table ->getSymbolFunType(lv->children.at(0)->token->tokenValue);
            if(lv->children.size() == 4) {
                used_par = 0;
            }
        } else if(prim -> children.at(0) ->parsingItem == ParsingItem::OverToken) {
            used_par = getExpFunType(prim->children.at(1), this_table);
        } else {
            used_par = 0;
        }
    } else {
        used_par = 0;
    }
    return used_par;
}


//基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number | Character
void ToPcodeVisitor::visit_PrimaryExp(Node *primary_exp, SymbolTable *this_table) {
    cout << "PrimaryExp" << endl;
    for(Node * child : primary_exp ->children) {
        if (child->parsingItem == ParsingItem::Exp) {
            visit_Exp_or_ConstExp(child, this_table);
        } else if(child -> parsingItem == ParsingItem::LVal) {
            visit_LVal_without_assign(child, this_table);
        } else if(child -> parsingItem == ParsingItem::Number) {
            addCode(CodeType::LDI, std::stoi(child->children.at(0)->token->tokenValue));
        } else if(child -> parsingItem == ParsingItem::Character) {
            std::string value = child->children.at(0)->token->tokenValue;
            if(value.size() == 4) {
                char c = value.at(2);
                if (c == 'n') {
                    addCode(CodeType::LDC,  "\n");
                } else if (c == 't') {
                    addCode(CodeType::LDC, "\t");
                } else if (c == 'a') {
                    addCode(CodeType::LDC, "\a");
                } else if(c == 'b') {
                    addCode(CodeType::LDC,  "\b");
                } else if(c == 'f') {
                    addCode(CodeType::LDC,  "\f");
                } else if(c == '0') {
                    addCode(CodeType::LDC, +"\0");
                } else if(c == 'v') {
                    addCode(CodeType::LDC,"\v");
                } else {
                    addCode(CodeType::LDC, value.substr(2,1));
                }
            } else {
                    addCode(CodeType::LDC, value.substr(1,1));
            };
        }
    }
}

//左值表达式 LVal → Ident ['[' Exp ']']
void ToPcodeVisitor::visit_LVal_without_assign(Node *lval, SymbolTable *this_table) {
    Node *ident = lval->children.at(0);
    std::string name = ident->token->tokenValue;

    Symbol *symbol = this_table->getSymbol(name);
    if(lval->children.size() > 1) {
        visit_Exp_or_ConstExp(lval->children.at(2), this_table);
    }
    //非左值，排除a;
    // 自定义符号
    //是数组，排除int a; fun(a);
    //没有数组下标，一定是int a[10]; fun(a);
    if((symbol->type == SymbolType::IntArray || symbol->type == SymbolType::CharArray)
        && lval->children.size() == 1) {
        //说明是数组名作为参数，进行特殊处理。
        addCode(CodeType::LDA, std ::to_string(symbol->blockNum) + "_"+name,0);
    } else {
        //其余的都需要调用数值，即使是a[i]作为参数，也需要传递数值
        addCode(CodeType::LOD, std ::to_string(symbol->blockNum) + "_"+name);
    }
}

//条件表达式 Cond → LOrExp
void ToPcodeVisitor::visit_Cond(Node *cond, SymbolTable *this_table,ForLabel *forLabel,IfLabel *ifLabel) {
    Node *lor_exp = cond->children.at(0);
    visit_LOrExp(lor_exp, this_table,forLabel,ifLabel);
}
//逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
void ToPcodeVisitor::visit_LOrExp(Node *lor_exp, SymbolTable *this_table,ForLabel *forLabel,IfLabel *ifLabel) {
    for(int i = 0; i < lor_exp->children.size(); ++i) {
        Node *child = lor_exp->children.at(i);
        if (child->parsingItem == ParsingItem::LAndExp) {
            TempLabel *tempLabel = new TempLabel();
            visit_LAndExp(child, this_table,tempLabel);
            addCode(CodeType::LABEL,tempLabel->label);
            if (i - 1 >= 0 && lor_exp->children.at(i - 1)->parsingItem == ParsingItem::OverToken) {
                if (lor_exp->children.at(i - 1)->token->tokenValue == "||") {
                    addCode(CodeType::OR);
                }
            }
        } else if (child->parsingItem == ParsingItem::LOrExp) {
            visit_LOrExp(child, this_table,forLabel,ifLabel);
            if(ifLabel != nullptr) {
                addCode(CodeType::JPF,ifLabel->if_stmt_label);
            } else {
                addCode(CodeType::JPF,forLabel->for_stmt_label);
            }
        }
    }
}


//LAndExp → EqExp | LAndExp '&&' EqExp
void ToPcodeVisitor::visit_LAndExp(Node *land_exp, SymbolTable *this_table,TempLabel *tempLabel) {
        for(int i = 0; i < land_exp->children.size(); ++i) {
            Node *child = land_exp->children.at(i);
            if (child->parsingItem == ParsingItem::EqExp) {
                visit_EqExp(child, this_table);
                if (i - 1 >= 0 && land_exp->children.at(i - 1)->parsingItem == ParsingItem::OverToken) {
                    if (land_exp->children.at(i - 1)->token->tokenValue == "&&") {
                        addCode(CodeType::AND);
                    }
                }
            } else if (child->parsingItem == ParsingItem::LAndExp) {
                visit_LAndExp(child, this_table,tempLabel);
                addCode(CodeType::JPC,tempLabel->label);
            }
        }
}

//EqExp → RelExp | EqExp ('==' | '!=') RelExp
void ToPcodeVisitor::visit_EqExp(Node *eq_exp, SymbolTable *this_table) {
    for(int i = 0; i < eq_exp->children.size(); ++i) {
        Node *child = eq_exp->children.at(i);
        if (child->parsingItem == ParsingItem::RelExp) {
            visit_RelExp(child, this_table);
            if (i - 1 >= 0 && eq_exp->children.at(i - 1)->parsingItem == ParsingItem::OverToken) {
                if (eq_exp->children.at(i - 1)->token->tokenValue == "==") {
                    addCode(CodeType::EQL);
                } else if (eq_exp->children.at(i - 1)->token->tokenValue == "!=") {
                    addCode(CodeType::NEQ);
                }
            }
        } else if (child->parsingItem == ParsingItem::EqExp) {
            visit_EqExp(child, this_table);
        }
    }
}

//RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
void ToPcodeVisitor::visit_RelExp(Node *rel_exp, SymbolTable *this_table) {
    for(int i = 0; i < rel_exp->children.size(); ++i) {
        Node *child = rel_exp->children.at(i);
        if (child->parsingItem == ParsingItem::AddExp) {
            visit_AddExp(child, this_table);
            if (i - 1 >= 0 && rel_exp->children.at(i - 1)->parsingItem == ParsingItem::OverToken) {
                if (rel_exp->children.at(i - 1)->token->tokenValue == "<") {
                    addCode(CodeType::LSS);
                } else if (rel_exp->children.at(i - 1)->token->tokenValue == ">") {
                    addCode(CodeType::GRT);
                } else if (rel_exp->children.at(i - 1)->token->tokenValue == "<=") {
                    addCode(CodeType::LER);
                } else if (rel_exp->children.at(i - 1)->token->tokenValue == ">=") {
                    addCode(CodeType::GEQ);
                }
            }
        } else if (child->parsingItem == ParsingItem::RelExp) {
            visit_RelExp(child, this_table);
        }
    }
}

void ToPcodeVisitor::addCode(CodeType type, std::string value) {
    PCode * code = new PCode(type,new std::string(value));
    code ->setCodeStr(codeTypeToStringMap.at(type) + " " + value);
    pCodeList -> push_back(code);
}

void ToPcodeVisitor::addCode(CodeType type, int value, int value2) {
    PCode * code = new PCode(type, new int(value), new int(value2));
    code ->setCodeStr(codeTypeToStringMap.at(type) + " " + std::to_string(value) + " " + std::to_string(value2));
    pCodeList -> push_back(code);
}

void ToPcodeVisitor::addCode(CodeType type) {
    PCode * code = new PCode(type);
    code ->setCodeStr(codeTypeToStringMap.at(type));
    pCodeList -> push_back(code);
}


void ToPcodeVisitor::addCode(CodeType type, int value) {
    PCode * code = new PCode(type, new int(value));
    code ->setCodeStr(codeTypeToStringMap.at(type) + " " + std::to_string(value));
    pCodeList -> push_back(code);
}

void ToPcodeVisitor::addCode(CodeType type,std::string value, int value2) {
    PCode * code = new PCode(type, new std::string(value), new int(value2));
    code ->setCodeStr(codeTypeToStringMap.at(type) + " " + value + " " + std::to_string(value2));
    pCodeList -> push_back(code);
}

void ToPcodeVisitor::printPcode() {
    cout << "\n\nPcode List:" << endl;
    int i = 0;
    for(PCode * code : *pCodeList) {
        cout <<i << ": " << code ->getCodeStr() << endl;
        i++;
    }
}

std :: vector<PCode*>* ToPcodeVisitor::getPcodeList() {
    return pCodeList;
}