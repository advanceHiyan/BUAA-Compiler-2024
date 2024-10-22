//
// Created by aba15 on 2024/10/15.
//

#include "Visitor.h"
#include "FileIO.h"
//编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block // 存在main函数
void SemanticAnalyzer::visit(Node *node) {
    overall_table = new OverallSymbolTable("global", nullptr,
                                         block_num++,TableType::OverallBlock);
    //除了初始可以用迭代器遍历，其它必须用size！
    int fuck = 0;
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
            int i = child->children.size() - 1;
            visit_Block(child->children.at(i), main_table);
            detectingFunReturnErrors(child->children.at(i),main_table);
        }
    }
    FileIO::printToFile_Symbol(&printf_list);
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//函数类型 FuncType → 'void' | 'int' | 'char'// 覆盖三种类型的函数
void SemanticAnalyzer::visit_FuncDef(Node *func_def) {
    Token *token = func_def->children.at(1)->token;
    std::string func_name = token->tokenValue;
    std::string void_int_char_str = func_def->children.at(0)->children.at(0)->token->tokenValue;
    SymbolType func_type = void_int_char_str =="void"? SymbolType::VoidFunc : (void_int_char_str ==
            "int"? SymbolType::IntFunc : SymbolType::CharFunc);
    TableType tableType = void_int_char_str =="void"? TableType::VoidFunc : (void_int_char_str ==
            "int"? TableType::IntFunc : TableType::CharFunc);
    Symbol *func_symbol = new Symbol(func_name, func_type, overall_table->BlockNum);
    printf_list.push_back(func_symbol);
    //函数名不添加到全局符号表
    FunSymbolTable *func_table = new FunSymbolTable(func_name, overall_table,
                                              block_num++,tableType);
    if(!overall_table->addFunTable(func_name,func_symbol,func_table) ) {
        FileIO::printToFile_Error(token->lineNumber,"b");//函数名重定义
    }
    visit_FParamsAndBlock(func_def, func_table);
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//FuncFParams → FuncFParam { ',' FuncFParam }
// FuncFParam → BType Ident ['[' ']']
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
void SemanticAnalyzer::visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table) {
    for (int i = 2; i < func_def->children.size(); i++) {
        if (func_def->children.at(i)->parsingItem == ParsingItem::FuncFParams) {
            Node *fparams = func_def->children.at(i);
            for(int j = 0; j < fparams->children.size(); j++) {
                if(fparams->children.at(j)->parsingItem == ParsingItem::FuncFParam) {
                    Node *fparam = fparams->children.at(j);
                    std::string  char_or_int = fparam->children.at(0)->children.at(0)->token->tokenValue;
                    std::string name = fparam->children.at(1)->token->tokenValue;
                    if(fparam->children.size() == 2) {
                        cout << "FuncFParam: " << char_or_int << " " << name << endl;
                        SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
                        printf_list.push_back(symbol);
                        this_table->paramTypes.push_back(type);
                        if(this_table->findSymbolInThis(name)) {
                            int line_num = fparam->children.at(1)->token->lineNumber;
                            FileIO::printToFile_Error(line_num,"b");//参数名重定义
                        }
                        this_table->addSymbol(symbol->name,symbol);
                    } else {
                        cout << "FuncFParam: " << char_or_int << " " << name << "[]" << endl;
                        SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
                        printf_list.push_back(symbol);
                        if(this_table->findSymbolInThis(name)) {
                            int line_num = fparam->children.at(1)->token->lineNumber;
                            FileIO::printToFile_Error(line_num,"b");//参数名重定义
                        }
                        this_table->paramTypes.push_back(type);
                        this_table->addSymbol(symbol->name,symbol);
                    }
                }
            }
        } else if (func_def->children.at(i)->parsingItem == ParsingItem::Block) {
            visit_Block(func_def->children.at(i), this_table);
            detectingFunReturnErrors(func_def->children.at(i), this_table);
        }
    }
}

void SemanticAnalyzer::detectingFunReturnErrors(Node *block, SymbolTable *this_table) {
    if(block ->children.size() == 2 || block->children.at(block->children.size() - 2) ->children.at(0) -> parsingItem
    == ParsingItem::Decl) {
        if(this_table->tableType != TableType::VoidFunc) {
            int line_num = block ->children.at(block ->children.size() - 1)->token->lineNumber;
            FileIO::printToFile_Error(line_num,"g");
        }
    } else {
        Node *stmt = block->children.at(block ->children.size() - 2) ->children.at(0);
        if(stmt->children.at(0)->token == nullptr || stmt->children.at(0)->token->tokenValue != "return") {
            if(this_table->tableType != TableType::VoidFunc) {
                int line_num = block ->children.at(block ->children.size() - 1)->token->lineNumber;
                FileIO::printToFile_Error(line_num,"g");
            }
        }
    }
}

//语句块 Block → '{' { BlockItem } '}'
//语句块项 BlockItem → Decl | Stmt
void SemanticAnalyzer::visit_Block(Node *block, SymbolTable *this_table) {
    for(int i = 1; i < block->children.size()-1; i++) {
        if(block->children.at(i)->parsingItem == ParsingItem::BlockItem) {
            Node *decl_or_stmt = block->children.at(i) ->children.at(0);
            if(decl_or_stmt->parsingItem == ParsingItem::Decl) {
                visit_Decl(decl_or_stmt, this_table);
            } else {
                visit_Stmt(decl_or_stmt, this_table);
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
void SemanticAnalyzer::visit_Stmt(Node *stmt, SymbolTable *this_table) {
    if(stmt->children.size() == 1 && stmt->children.at(0)->parsingItem == ParsingItem::OverToken) {//;
        return;//空语句
    } else if(stmt->children.at(0)->parsingItem == ParsingItem::Block) {//if{}else{} 空{}
        SymbolTable *new_table = new SymbolTable("emptyTable", this_table,
                                                 block_num++,TableType::EmptyBlock);
        visit_Block(stmt->children.at(0), new_table);
    } else if (stmt->children.at(0)->parsingItem == ParsingItem::LVal) {
        if(stmt->children.at(2) ->parsingItem == ParsingItem::Exp) {//LVal = Exp;
            visit_LVal(stmt->children.at(0), this_table);
            visit_Exp_or_ConstExp(stmt->children.at(2), this_table);
        } else {//LVal = getint();LVal = getchar();
            visit_LVal(stmt->children.at(0), this_table);
        }
    } else if(stmt ->children.at(0) ->parsingItem == ParsingItem::Exp) {//exp;
        visit_Exp_or_ConstExp(stmt->children.at(0), this_table);
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "if") {
        visit_If_Stmt(stmt, this_table);
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "for") {
        visit_For_Stmt(stmt, this_table);
    } else if(stmt->children.at(0)->token != nullptr && (stmt->children.at(0)->token->
    tokenValue == "continue" ||stmt->children.at(0)->token->tokenValue == "break")) {
        if(!this_table->isInFor()) {
            int line_num = stmt->children.at(0)->token->lineNumber;
            FileIO::printToFile_Error(line_num,"m");//continue/break语句不在for循环中
        }
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "return") {
        int ret = this_table->isReturnFun();
        if(ret == -1) {//no
            int line_num = stmt->children.at(0)->token->lineNumber;
            FileIO::printToFile_Error(line_num,"f");//return语句不在函数中
        } else if(ret == 2 && stmt->children.size() > 2) {
            int line_num = stmt->children.at(0)->token->lineNumber;
            FileIO::printToFile_Error(line_num,"f");//return语句不在函数中
        }
    } else if(stmt->children.at(0)->token != nullptr && stmt->children.at(0)->token->tokenValue == "printf") {
        detecting_printf_errors(stmt, this_table);
        for(int i = 2; i < stmt->children.size(); i++) {
            if(stmt->children.at(i)->parsingItem == ParsingItem::Exp) {
                visit_Exp_or_ConstExp(stmt->children.at(i), this_table);
            }
        }
    }
    else {
        cout << "Stmt ???" << endl;
    }
}

//| 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt // 1. 无缺省，1种情况 2.
// ForStmt → LVal '=' Exp
void SemanticAnalyzer::visit_For_Stmt(Node *stmt, SymbolTable *this_table) {
    int i = 2;
    while(i < stmt ->children.size()) {
        if(stmt->children.at(i)->parsingItem == ParsingItem::ForStmt) {
            visit_LVal(stmt->children.at(i) ->children.at(0), this_table);
            visit_Exp_or_ConstExp(stmt->children.at(i) ->children.at(2), this_table);
        } else if (stmt->children.at(i)->parsingItem == ParsingItem::Stmt) {
            if(stmt->children.at(i)->children.at(0) ->parsingItem == ParsingItem::Block) {
                SymbolTable *new_table = new SymbolTable("forTable", this_table,
                                                         block_num++,TableType::ForBlock);
                visit_Block(stmt->children.at(i)->children.at(0), new_table);
            } else {
                visit_Stmt(stmt->children.at(i), this_table);
            }
        } else if(stmt->children.at(i)->parsingItem == ParsingItem::Cond) {
            visit_Cond(stmt->children.at(i), this_table);
        }
        i += 1;
    }
}

//左值表达式 LVal → Ident ['[' Exp ']']
void SemanticAnalyzer::visit_LVal(Node *lval, SymbolTable *this_table) {
    Node *ident = lval->children.at(0);
    std::string name = ident->token->tokenValue;

    int line_num = ident->token->lineNumber;
    int ret = this_table->findSymbolInUpperFloor(name);
    if(ret == -1) { //没
        FileIO::printToFile_Error(line_num,"c");
    } else if(ret == 2) {
        FileIO::printToFile_Error(line_num,"h");
    }
    if(lval->children.size() > 1) {
        visit_Exp_or_ConstExp(lval->children.at(2), this_table);
    }
}


int countOccurrences(const std::string& str, const std::string& subStr) {
    int count = 0;
    size_t pos = 0;
    while ((pos = str.find(subStr, pos))!= std::string::npos) {
        ++count;
        pos += subStr.length();
    }
    return count;
}

void SemanticAnalyzer::visit_InitVal(Node *init_val, SymbolTable *this_table) {
    for(int i = 0; i < init_val->children.size(); i++) {
        if(init_val->children.at(i)->parsingItem == ParsingItem::ConstExp ||
        init_val->children.at(i)->parsingItem == ParsingItem ::Exp ) {
            visit_Exp_or_ConstExp(init_val->children.at(i), this_table);
        }
    }
}

//| 'printf''('StringConst {','Exp}')'';' // 1.有Exp 2.无Exp
void SemanticAnalyzer::detecting_printf_errors(Node *stmt, SymbolTable *this_table) {
    std::string string_const = stmt->children.at(2)->token->tokenValue;
    int exp_num = (stmt->children.size() - 5) / 2;
    int count = countOccurrences(string_const, "%c") + countOccurrences(string_const, "%d");
    int line_num = stmt->children.at(0)->token->lineNumber;
    if(exp_num!= count) {
        FileIO::printToFile_Error(line_num,"l");//printf参数个数错误
    }
}

//| 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
void SemanticAnalyzer::visit_If_Stmt(Node *stmt, SymbolTable *this_table) {
    int i = 1;
    while(i < stmt ->children.size()) {
        if(stmt->children.at(i)->parsingItem == ParsingItem::Stmt) {
            visit_Stmt(stmt->children.at(i), this_table);//Block可能是{},直接识别为空快，if的块不需要特殊处理
        } else if(stmt->children.at(i)->parsingItem == ParsingItem::Cond) {
            visit_Cond(stmt->children.at(i), this_table);
        }
        i += 1;
    }
}

//声明 Decl → ConstDecl | VarDecl
//常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
//变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
void SemanticAnalyzer::visit_Decl(Node *node, SymbolTable *this_table) {
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
void SemanticAnalyzer::visit_def(Node *node, SymbolTable *this_table,std::string char_or_int) {
    if (node->parsingItem == ParsingItem::ConstDef) {//常量
        std::string name = node ->children.at(0) ->token->tokenValue;
        if(node->children.at(2)->parsingItem == ParsingItem::ConstExp) {//数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::ConstIntArray : SymbolType::ConstCharArray;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            if(this_table->findSymbolInThis(name)) {
                FileIO::printToFile_Error(node->children.at(0)->token->lineNumber,"b");//变量名重定义
            }
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
            visit_Exp_or_ConstExp(node->children.at(2), this_table);//!!!!!!
        } else {//不是数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::ConstInt : SymbolType::ConstChar;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            if(this_table->findSymbolInThis(name)) {
                FileIO::printToFile_Error(node->children.at(0)->token->lineNumber,"b");//变量名重定义
            }
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        }
    } else {//变量
        std::string name = node ->children.at(0) ->token->tokenValue;
        if(node ->children.size() > 1 && node->children.at(2)->parsingItem == ParsingItem::ConstExp) {//数组
            //非法指针引发的错误！！！！
            SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            if(this_table->findSymbolInThis(name)) {
                FileIO::printToFile_Error(node->children.at(0)->token->lineNumber,"b");//变量名重定义
            }
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
            visit_Exp_or_ConstExp(node->children.at(2), this_table);//!!!!!!
        } else {//不是数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            if(this_table->findSymbolInThis(name)) {
                FileIO::printToFile_Error(node->children.at(0)->token->lineNumber,"b");//变量名重定义
            }
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        }

        if(node->children.at(node->children.size()-1) ->parsingItem == ParsingItem::InitVal ||
        node->children.at(node->children.size()-1) ->parsingItem == ParsingItem::ConstInitVal) {
            visit_InitVal(node->children.at(node->children.size()-1), this_table);
        } //!!!!!
    }
}

//表达式 Exp → AddExp // 存在即可

void SemanticAnalyzer::visit_Exp_or_ConstExp(Node *exp, SymbolTable *this_table) {
    Node *add_exp = exp->children.at(0);
    visit_AddExp(add_exp, this_table);
}

//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
void SemanticAnalyzer::visit_AddExp(Node *add_exp, SymbolTable *this_table) {
    for (Node *child : add_exp->children) {
        if(child->parsingItem == ParsingItem::MulExp) {
            visit_MulExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::AddExp) {
            visit_AddExp(child,this_table);
        }
    }
}

//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
void SemanticAnalyzer ::visit_MulExp(Node * mulexp,SymbolTable * this_table) {
    for(Node * child : mulexp ->children) {
        if (child->parsingItem == ParsingItem::MulExp) {
            visit_MulExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::UnaryExp) {
            visit_Unary(child,this_table);
        }
    }
}

//UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
//FuncRParams → Exp { ',' Exp }
void SemanticAnalyzer::visit_Unary(Node * unary,SymbolTable * this_table) {
    if(unary->children.at(0)->parsingItem == ParsingItem::OverToken) {//Ident
        Node * ident = unary->children.at(0);
        int ret = this_table->findSymbolInUpperFloor(ident->token->tokenValue);
        int line_num = ident ->token->lineNumber;
        if(ret == -1) {
            FileIO::printToFile_Error(line_num,"c");//变量未声明
            return;//未定义的符号
        }
        std :: vector<SymbolType> * funcParams = overall_table ->findFunType(ident->token->tokenValue);
        if(funcParams != nullptr) {
            int usedFun_num = 0;
            if(unary->children.size() == 4) {
                usedFun_num = unary->children.at(2)->children.size() / 2 + 1;//exp,exp……
            }
            if(usedFun_num != funcParams->size()) {//参数个数不匹配
                FileIO::printToFile_Error(line_num,"d");
            } else {
                if(funcParams->empty()) {//空的
                    return;
                } else {
                    detectingFunRParamsErrors(unary, line_num, funcParams, this_table);
                    cout << "fuck 7 " << endl;
                }
            }
            if(usedFun_num > 0) {
                Node *fun_am = unary->children.at(2);
                    for(Node * child : fun_am->children) {
                        if (child->parsingItem == ParsingItem::Exp) {
                            visit_Exp_or_ConstExp(child, this_table);
                        }
                    }
            }
            cout << "fuck 8 " << endl;
        } else {
            cout << "error nullptr in fun" << endl;
        }
    }
    for(Node * child : unary ->children) {
        if (child->parsingItem == ParsingItem::PrimaryExp) {
            visit_PrimaryExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::UnaryExp) {
            visit_Unary(child, this_table);
        }
    }
}
//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
//UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
//FuncRParams → Exp { ',' Exp }
//基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number | Character
//左值表达式 LVal → Ident ['[' Exp ']']
void SemanticAnalyzer::detectingFunRParamsErrors(Node *unaryExp, int line_num,std::vector<SymbolType> * params,
SymbolTable * this_table) {
    Node *ps = unaryExp->children.at(2);
    for(int i = 0;i < params->size();i++) {
        cout<< params->size() << " " << i << endl;
        cout << "sybtype:"  << endl;
        Node * add_exp = ps ->children.at(i * 2) ->children.at(0);
        Node *mul_exp = add_exp->children.at(add_exp->children.size() - 1);
        Node *uy = mul_exp -> children.at(mul_exp->children.size() - 1);
        int used_par = -1;
        if(uy -> children.at(0) ->parsingItem == ParsingItem::OverToken) {//变量
            used_par = 0;
        } else if(uy ->children.at(0) ->parsingItem == ParsingItem::PrimaryExp) {
            Node * prim = uy ->children.at(0);
            if(prim -> children.at(0) ->parsingItem == ParsingItem::LVal) {
                cout << "fuck 1 " << endl;
                Node * lv = prim -> children.at(0);
                used_par = this_table ->getSymbolFunType(lv->children.at(0)->token->tokenValue);
                if(lv->children.size() == 4) {
                    used_par = 0;
                }
            } else if(prim -> children.at(0) ->parsingItem == ParsingItem::OverToken) {
                cout << "fuck 2 " << endl;
                used_par = getExpFunType(prim->children.at(1), this_table);
            } else {
                cout << "fuck 3 " << endl;
                used_par = 0;
            }
        } else {
            used_par = 0;
        }
        cout << "used_par:" << used_par << endl;
        if(used_par == -1) {
            cout << "error in detectingFunRParamsErrors" << endl;
        } else if(used_par == 0 && params->at(i) != SymbolType::Int && params->at(i) != SymbolType::Char) {
            FileIO::printToFile_Error(line_num,"e");//参数类型错误
        } else if(used_par == 1 && params->at(i) != SymbolType::IntArray) {
            FileIO::printToFile_Error(line_num,"e");//参数类型错误
        } else if(used_par == 2 && params->at(i) != SymbolType::CharArray) {
            FileIO::printToFile_Error(line_num,"e");//参数类型错误
        } else {
            cout << "fuck 4 " << endl;
//            continue;
        }
        cout << "fuck 5 " << endl;
    }
    cout << "fuck 6 " << endl;
}

int SemanticAnalyzer::getExpFunType(Node *exp, SymbolTable *this_table) {
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
void SemanticAnalyzer::visit_PrimaryExp(Node *primary_exp, SymbolTable *this_table) {
    for(Node * child : primary_exp ->children) {
        if (child->parsingItem == ParsingItem::Exp) {
            visit_Exp_or_ConstExp(child, this_table);
        } else if(child -> parsingItem == ParsingItem::LVal) {
            visit_LVal_without_assign(child, this_table);
        }
    }
}

//左值表达式 LVal → Ident ['[' Exp ']']
void SemanticAnalyzer::visit_LVal_without_assign(Node *lval, SymbolTable *this_table) {
    Node *ident = lval->children.at(0);
    std::string name = ident->token->tokenValue;

    int line_num = ident->token->lineNumber;
    int ret = this_table->findSymbolInUpperFloor(name);
    if(ret == -1) { //没
        FileIO::printToFile_Error(line_num,"c");
    }
    if(lval->children.size() > 1) {
        visit_Exp_or_ConstExp(lval->children.at(2), this_table);
    }
}

//条件表达式 Cond → LOrExp
void SemanticAnalyzer::visit_Cond(Node *cond, SymbolTable *this_table) {
    Node *lor_exp = cond->children.at(0);
    visit_LOrExp(lor_exp, this_table);
}
//逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
void SemanticAnalyzer::visit_LOrExp(Node *lor_exp, SymbolTable *this_table) {
    for(Node * child : lor_exp ->children) {
        if (child->parsingItem == ParsingItem::LOrExp) {
            visit_LOrExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::LAndExp) {
            visit_LAndExp(child,this_table);
        }
    }
}

//LAndExp → EqExp | LAndExp '&&' EqExp
void SemanticAnalyzer::visit_LAndExp(Node *land_exp, SymbolTable *this_table) {
    for(Node * child : land_exp ->children) {
        if (child->parsingItem == ParsingItem::LAndExp) {
            visit_LAndExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::EqExp) {
            visit_EqExp(child,this_table);
        }
    }
}

//EqExp → RelExp | EqExp ('==' | '!=') RelExp
void SemanticAnalyzer::visit_EqExp(Node *eq_exp, SymbolTable *this_table) {
    for(Node * child : eq_exp ->children) {
        if (child->parsingItem == ParsingItem::EqExp) {
            visit_EqExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::RelExp) {
            visit_RelExp(child,this_table);
        }
    }
}

//RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
void SemanticAnalyzer::visit_RelExp(Node *rel_exp, SymbolTable *this_table) {
    for(Node * child : rel_exp ->children) {
        if (child->parsingItem == ParsingItem::RelExp) {
            visit_RelExp(child,this_table);
        } else if(child -> parsingItem == ParsingItem::AddExp) {
            visit_AddExp(child,this_table);
        }
    }
}