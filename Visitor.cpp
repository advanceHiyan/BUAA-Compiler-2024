//
// Created by aba15 on 2024/10/15.
//

#include "Visitor.h"
#include "FileIO.h"
//编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block // 存在main函数
void SemanticAnalyzer::visit(Node *node) {
    SymbolTable *table = new SymbolTable("global", nullptr,
                                         block_num++,TableType::OverallBlock);
    //除了初始可以用迭代器遍历，其它必须用size！
    for (auto child : node->children) {
        if (child ->parsingItem == ParsingItem::Decl) {
            cout << "Decl" << endl;
            visit_Decl(child, table);
        } else if(child ->parsingItem == ParsingItem::FuncDef){
            cout << "FuncDef" << endl;
            visit_FuncDef(child, table);
        } else {// MainFuncDef 函数名不算全局Symbol
            SymbolTable *main_table = new SymbolTable("main", table,
                                                      block_num++,TableType::IntFunc);
            cout << "MainFuncDef" << endl;
            int i = child->children.size() - 1;
            visit_Block(child->children[i], main_table);
        }
    }
    FileIO::printToFile_Symbol(&printf_list);
    cout << " printf_list size: " <<printf_list.size() << endl;
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//函数类型 FuncType → 'void' | 'int' | 'char'// 覆盖三种类型的函数
void SemanticAnalyzer::visit_FuncDef(Node *func_def, SymbolTable *this_table) {
    std::string func_name = func_def->children[1]->token->tokenValue;
    SymbolType func_type = func_def->children[0]->children[0]->token->tokenValue ==
            "void"? SymbolType::VoidFunc : (func_def->children[0]->children[0]->token->tokenValue ==
            "int"? SymbolType::IntFunc : SymbolType::CharFunc);
    TableType tableType = func_def->children[0]->children[0]->token->tokenValue ==
            "void"? TableType::VoidFunc : (func_def->children[0]->children[0]->token->tokenValue ==
            "int"? TableType::IntFunc : TableType::CharFunc);
    cout << "FuncDef: " << func_name << " " << func_def->children[0]->children[0]->token->tokenValue << endl;
    Symbol *func_symbol = new Symbol(func_name, func_type, this_table->BlockNum);
    printf_list.push_back(func_symbol);
    this_table->addSymbol(func_symbol->name, func_symbol);
    SymbolTable *func_table = new SymbolTable(func_name, this_table,
                                              block_num++,tableType);
    visit_FParamsAndBlock(func_def, func_table);
}

//函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
//FuncFParams → FuncFParam { ',' FuncFParam }
// FuncFParam → BType Ident ['[' ']']
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
void SemanticAnalyzer::visit_FParamsAndBlock(Node *func_def, SymbolTable *this_table) {

    for (int i = 2; i < func_def->children.size(); i++) {
        if (func_def->children[i]->parsingItem == ParsingItem::FuncFParams) {
            Node *fparams = func_def->children[i];
            for(int j = 0; j < fparams->children.size(); j++) {
                if(fparams->children[j]->parsingItem == ParsingItem::FuncFParam) {
                    Node *fparam = fparams->children[j];
                    std::string  char_or_int = fparam->children[0]->children[0]->token->tokenValue;
                    std::string name = fparam->children[1]->token->tokenValue;
                    if(fparam->children.size() == 2) {
                        cout << "FuncFParam: " << char_or_int << " " << name << endl;
                        SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
                        printf_list.push_back(symbol);
                        this_table->addSymbol(symbol->name,symbol);
                    } else {
                        cout << "FuncFParam: " << char_or_int << " " << name << "[]" << endl;
                        SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
                        Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
                        printf_list.push_back(symbol);
                        this_table->addSymbol(symbol->name,symbol);
                    }
                }
            }
        } else if (func_def->children[i]->parsingItem == ParsingItem::Block) {
            cout << "fun-Block" << endl;
            visit_Block(func_def->children[i], this_table);
        }
    }
}

//语句块 Block → '{' { BlockItem } '}'
//语句块项 BlockItem → Decl | Stmt
void SemanticAnalyzer::visit_Block(Node *block, SymbolTable *this_table) {
    for(int i = 1; i < block->children.size()-1; i++) {
        if(block->children[i]->parsingItem == ParsingItem::BlockItem) {
            Node *decl_or_stmt = block->children[i] ->children[0];
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
    if(stmt->children[0]->parsingItem == ParsingItem::Block) {
        SymbolTable *new_table = new SymbolTable("emptyTable", this_table,
                                                 block_num++,TableType::EmptyBlock);
        visit_Block(stmt->children[0], new_table);
    } else if(stmt->children[0]->token != nullptr && stmt->children[0]->token->tokenValue == "if") {
        int i = 1;
        while(i < stmt ->children.size()) {
            //TODO
            if(stmt->children[i]->parsingItem == ParsingItem::Stmt) {
                visit_Stmt(stmt->children[i], this_table);
            }
            i += 1;
        }
    } else if(stmt->children[0]->token != nullptr && stmt->children[0]->token->tokenValue == "for") {
        int i = 1;
        //TODO int i = 1;
        while(i < stmt ->children.size()) {
            if (stmt->children[i]->parsingItem == ParsingItem::Stmt) {
                if(stmt->children[i]->children[0] ->parsingItem == ParsingItem::Block) {
                    SymbolTable *new_table = new SymbolTable("forTable", this_table,
                                                             block_num++,TableType::ForBlock);
                    visit_Block(stmt->children[i]->children[0], new_table);
                    cout << "forTable" << endl;
                } else {
                    //TODO
                }
            }
            i += 1;
        }
    }
    else {
        cout << "Stmt TODO" << endl;
    }
}

//声明 Decl → ConstDecl | VarDecl
//常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
//基本类型 BType → 'int' | 'char' // 覆盖两种数据类型的定义
//变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
void SemanticAnalyzer::visit_Decl(Node *node, SymbolTable *this_table) {
    if (node->children[0] ->parsingItem == ParsingItem::ConstDecl) {
        Node *const_decl = node->children[0];
        std::string  char_or_int = const_decl->children[1]->children[0]->token->tokenValue;
        for (auto const_def : const_decl->children) {
            if(const_def -> parsingItem == ParsingItem::ConstDef) {
                visit_def(const_def, this_table,char_or_int);
            }
        }
    } else {
        Node *var_decl = node->children[0];
        std::string  char_or_int = var_decl->children[0]->children[0]->token->tokenValue;
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
        std::string name = node ->children[0] ->token->tokenValue;
        if(node->children[2]->parsingItem == ParsingItem::ConstExp) {//数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::ConstIntArray : SymbolType::ConstCharArray;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        } else {//不是数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::ConstInt : SymbolType::ConstChar;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        }
    } else {//变量
        std::string name = node ->children[0] ->token->tokenValue;
        if(node->children[2]->parsingItem == ParsingItem::ConstExp) {//数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::IntArray : SymbolType::CharArray;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        } else {//不是数组
            SymbolType type =  char_or_int == "int"  ? SymbolType::Int : SymbolType::Char;
            Symbol *symbol = new Symbol(name,type,this_table->BlockNum);
            printf_list.push_back(symbol);
            this_table->addSymbol(symbol->name,symbol);
        }
    }
}
