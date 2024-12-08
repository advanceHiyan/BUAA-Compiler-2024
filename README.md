# <center>北航编译课程设计文档

## 源代码SysY中间代码PCode

这个编译器会编译SysY语言为PCode中间代码，并解释执行。  

SysY语言是C语言子集，文法见"文法.pdf"。  

PCode见下文介绍。

## 目录

- [北航编译课程设计文档](#北航编译课程设计文档)
  - [源代码SysY中间代码PCode](#源代码sysy中间代码pcode)
  - [目录](#目录)
  - [编译器总体设计](#编译器总体设计)
  - [词法分析设计](#词法分析设计)
  - [语法分析设计](#语法分析设计)
  - [语义分析设计](#语义分析设计)
  - [代码生成设计](#代码生成设计)
    - [代码生成部分](#代码生成部分)
    - [解释执行部分](#解释执行部分)
    - [代码生成与解释执行的难点](#代码生成与解释执行的难点)

## 编译器总体设计

这是一个用C++实现的编译SysY语言到PCode并解释执行的编译器。  
编译器的总体设计分为词法分析、语法分析、语义分析、代码生成四个阶段。  

```

//main.cpp
int main() {
    std::string content = FileIO::openFile();
    Lexer lexer(content, reserveToConstMap); //词法分析器
    Parser parser(lexer); //语法分析器，生成语法树
    Node* root = parser.parse();
    Visitor *semanticAnalyzer = new SemanticAnalyzer();//语义分析器，负责检查语义错误
    (*semanticAnalyzer).visit(root);
    if (!FileIO::hasError) { //如果没有语义错误就进行代码生成
        Visitor *codeGenerator = new ToPcodeVisitor(); //代码生成器，负责生成PCode
        (*codeGenerator).visit(root);
        ToPcodeVisitor * pcodeGenerator = static_cast<ToPcodeVisitor*>(codeGenerator); 
        Interpreter *interpreter = new Interpreter (pcodeGenerator->getPcodeList()); //解释器，负责执行PCode
        interpreter ->interpret();
    }
    FileIO::closeFile();
    return 0;
}
```
下面是IDE视角的编译器源代码  

![961bb3904037f3a04dd5031dda53f8e](https://github.com/user-attachments/assets/0e077668-c3a5-4ff2-8bbe-8b9830fd6ae6)


- main.cpp：程序入口
- fileIO/ 文件读写和enum class定义模块
- lexer/ 词法分析器模块
- parser/ 语法分析器模块
- semantic/ 语义分析器模块
- codeGeneration/ 代码生成器模块和解释器模块


## 词法分析设计

最简单的一个模块，只需要设计一个有限状态机，或者直接解析字符。  
只有两个值得注意
一个是**转义字符的处理**。因为testfile.txt中存储的
```
print("hello,world\n");
```
换行符\n实际是是两个字符: '\\'和'n',而我读取的时候也是按照两个字符存储的，在代码生成解释执行阶段要额外注意这一点。  
在词法分析中读取到'\\'我会直接把它后面一个字符一起提取出来了。省去很多麻烦。

```
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
```

还有一个是空白字符，注释等的处理。  
我想了一个笨方法：
```
void Lexer:: goSpacetoNext() {
    while (curPos < content.size() && std::isspace(content[curPos])) {
        if (content[curPos] == '\n') {
            lineNum++;
        }
        curPos++; // 忽略空白字符
    }

    if(curPos < content.size() && content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '/') {
        //TODO ：单行注释
        goSpacetoNext();
    }
    else if(curPos < content.size() && content[curPos] == '/' && curPos + 1 < content.size() && content[curPos + 1] == '*') {
        //TODO :多行注释
        goSpacetoNext();
    }
    return ;
}
```

它的调用方式是这样：
```
int Lexer::next() {
    canUse = false; //使用这个接口，就无法建立Tokens
    goSpacetoNext();

    if (curPos >= content.size()) {
        curLexType = ConstType::OVERFILE; // 文件结束
        curToken = "";
        return -1;
    }

    char ch = content[curPos];
    // TODO: 实现其他的token类型
}
```

## 语法分析设计

根据文法递归下降生成语法树。  
首先定义节点
```
// parser/Node.h
#include "../fileIO/ConstType.h"
struct Token {
    ConstType tokenType;
    std::string tokenValue;
    int lineNumber;
};

struct Node {
    ParsingItem parsingItem;
    Token *token; // 终结符
    std::vector<Node *> children;
}
```
然后生成语法树
看下面这个例子：

```
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
```
唯一需要注意的是左递归文法，我是这么处理的：
```
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
```


## 语义分析设计

一般来说，语义分析是要遍历语法树的，当然也可以选择在建立语法树的时候就进行语义分析。  
而遍历语法树有两种方式。

一种是给节点定义向下遍历的方法，很自然的递归下降。：
而第二种方式则是将 Node 作为参数。为了避免使用全局变量，我们需要创建一个新的类 Visitor。
这里我选择了第二种方式。
正如教程所说：
“*那么就这两种方法来说，哪种更加适合抽象语法树的遍历呢？答案是第二种使用 Visitor 的方法。因为语义分析的关键是跨节点的语义信息传递。采取第一种方法时，信息被隔离在了每一个节点之中，想要传播只能作为 visit 函数的参数和返回值，沿着节点的遍历顺序进行传递。而对于第二种方法，类中所有方法都共享一个类作用域，这意味着信息不仅能沿着遍历顺序传递，还能跨越语法树的分支传播到需要的地方，而不需要经过层层调用。这种功能十分重要，因为实现上下文关系的关键，符号，就是跨越语法树分支存在的。*”

首先定义 Visitor 类，并定义 visit 方法。

```
class  Visitor { // 遍历树，不论度是多少，都能共享Visitor类空间
public:
    virtual void visit(Node* node) = 0; // 定义一个虚函数，用于遍历树 ,=0表示函数在基类没有定义，所有派生类必须实现该函数
};
```

```
class SemanticAnalyzer : public Visitor { // 语义分析
public:
    void visit(Node *node) override; // 重载visit函数，实现语义分析，public对外接口

protected:
    OverallSymbolTable *overall_table; // 全局符号表
    int block_num = 1;//记录用了多少个块号，只用于建立新符号表，不可用于其它参数
    std::vector<Symbol*> printf_list; // 用于记录待输出语句及其块号

    void visit_Decl(Node *node, SymbolTable *this_table);

    void visit_def(Node *node, SymbolTable *this_table, std::string char_or_int);

    void visit_FuncDef(Node *func_def);

    void visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table);

    void visit_Block(Node *block, SymbolTable *this_table);

    void visit_Stmt(Node *stmt, SymbolTable *this_table);

    //其它可能需要实现的函数，不一定每一类节点都要实现一个visit函数
};
```


语义分析需要用到符号表，可以自行设计符号表，很简单，没什么好说的。


## 代码生成设计

### 代码生成部分

本人选择了pcode，pcode比较自由，我只选择了其中一些指令，加上自己设计的一些指令。
```
enum  class CodeType {
    LABEL,
    // 声明一个变量，用于在程序中定义新的变量
    VARINT, VARCHAR, VARINTARRAY, VARCHARARRAY,
    // 将值数字、char压入栈顶
    LDI, LDC,
    PAL, //popValue then popAddress then setValue to address
    // 加法操作，将两个值相加
    ADD,SUB,
    // 乘法操作，将两个值相乘
    MUL,DIV,MOD,
    // 逻辑非操作，取负
    NOT,MUS,
    // 主函数标签，用于标识程序的主函数开始位置
    MAIN,
    // 函数标签，用于标识一个函数的开始位置
    FUNC,
    // 函数返回，可以选择是否返回一个值
    RET,
    // 定义函数参数，包括参数名和参数类型
    PARINT,PARCHAR,PARINTARRAY,PARCHARARRAY,
    // 函数调用，调用指定名称的函数
    CAL,
    // 传递参数、传递函数参数
    APR,  APA,
    // 获取一个整数并将其压入栈顶
    GETINT,
    GETCHAR,
    // 弹出栈中的值并打印，可能涉及字符串和参数数量
    PRINTF,
    // 获取变量的值，指定变量名和变量类型
    LOD,
    LDA,// 获取变量的地址，指定变量名和变量类型
    EMPTY,// 给数组空间占位
    EXF,//标记函数结束，用于无返回值的void函数
    // 退出程序
    EXIT,
    JMP, //无条件跳转
    JPC, //如果栈顶为0，跳转
    JPF, //如果栈顶为非0，跳转
    OR,AND,
    EQL, //判断两个值是否相等
    NEQ, //判断两个值是否不相等
    LSS, //判断第一个值是否小于第二个值
    LER, //判断第一个值是否小于等于第二个值
    GRT, //判断第一个值是否大于第二个值
    GEQ, //判断第一个值是否大于等于第二个值
};
```

其实语义分析的时候就可以生成中间代码了，甚至语法分析的时候就可以了，但是本人为了代码美观，并且为了方便以后把代码放到github上，帮助其他同学，所以语义分析和中间代码生成，遍历了两遍语法树。  

这是代码生成的visitor类：
```
class ToPcodeVisitor : public Visitor {
public:
    void visit(Node *node) override; // 重载visit函数，实现代码生成
    std :: vector<PCode*>* getPcodeList(); // 获取生成的pcode语句
protected:
    OverallSymbolTable *overall_table; // 全局符号表
    int block_num = 1;//记录用了多少个块号，只用于建立新符号表，不可用于其它参数
    std :: vector<PCode*>* pCodeList = new std :: vector<PCode*>; // 存放生成的pcode语句,不初始化会报错

    void visit_Decl(Node *node, SymbolTable *this_table);

    void visit_def(Node *node, SymbolTable *this_table, std::string char_or_int);

    void visit_FuncDef(Node *func_def);

    void visit_FParamsAndBlock(Node *func_def, FunSymbolTable *this_table);

    void visit_Block(Node *block, SymbolTable *this_table,ForLabel *forLabel);

    void visit_Stmt(Node *stmt, SymbolTable *this_table,ForLabel *forLabel);

    //其它需要实现的方法
}
```

**注意代码生成需要用到符号表，不需要用到栈，代码解释执行的时候需要用到栈不需要符号表！**

代码生成和解释执行密不可分，在代码生成的过程中，你需要考虑代码解释执行时栈的变化，而在解释执行时，又要考虑代码生成的顺序。

因此，我并没有按照先生成代码，再设计解释器的顺序，而是挑选了一些简单的文法，一边生成代码，一边设计解释器。等这些简单的文法跑通了再想那些难的。

本人建议先从
```
编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
声明 Decl → ConstDecl | VarDecl
……
函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
……
语句块项 BlockItem → Decl | Stmt
语句 Stmt → LVal '=' Exp ';' // 每种类型的语句都要覆盖
| [Exp] ';' //有无Exp两种情况
| Block
| 'return' [Exp] ';' // 1.有Exp 2.无Exp
| LVal '=' 'getint''('')'';'
| LVal '=' 'getchar''('')'';'
| 'printf''('StringConst {','Exp}')'';'
……
加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp

```
入手，不要涉及if for break continue cond 等语句
等这些简单的文法跑通了再想那些难的。

### 解释执行部分

解释执行部分，就没有语义分析和代码生成的 作用域块和对应的符号表了。  

代码生成的时候，需要按照每个代码块进行分析。  
解释执行的时候，没有代码块，只有函数块和栈。

举个例子：
对于源代码
```
int a = 1;

int fun() {
    int b = 2;
    return b;
}

int main() {
    if(a == 1) {
        printf("hello,world\n");
    }
    {}
    fun();
    return 0;
}
```
对于语义分析和代码生成，有全局、fun{}、main{},if{},{}
五个代码块。

对于解释执行，只有全局、main，fun三个函数块。
也就是说，解释执行的时候，只有函数调用才会需要你考虑一些类似于转换现场的东西。

同时，因为没有指针或malloc()，所以不需要堆，**只有栈**。

### 代码生成与解释执行的难点

首先是数组 还有 char的存入 
数组需要用EMPTY指令站位。
**char直接按照int**存入！
注意中间转义字符的处理！
```
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
//常量定义 ConstDef → Ident [ '[' ConstExp ']' ] '=' ConstInitVal
//变量定义 VarDef → Ident [ '[' ConstExp ']' ] | Ident [ '[' ConstExp ']' ] '='InitVal // 包含普通常量、一维数组定义
void ToPcodeVisitor::visit_def(Node *node, SymbolTable *this_table,std::string char_or_int) {
    cout << "def" << endl;
    std::string name = node ->children.at(0) ->token->tokenValue;
    if (true) {//不区分常量变量
        if(node->children.size() > 2 && node->children.at(2)->parsingItem == ParsingItem::ConstExp) {//数组TODO
        } else {//不是数组TODO
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
```
下面看EMPTY的解释执行逻辑
```
            case CodeType::EMPTY :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                int v2 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                VarInfo varInfo = getVarInfo(*strPtr);
                if (varInfo.isArray) {
                    std::vector<int> valueEDs;
                    for (int i = 0; i < v2; i++) {
                        valueEDs.push_back(pop());
                    }
                    int needSize = (pop());
                    for(int i = valueEDs.size() - 1; i >= 0; i--) {
                        push(valueEDs.at(i));
                    }
                    for (int i = 0; i < needSize - valueEDs.size(); i++) {
                        push(0);
                    }
                } else {
                    if (v2 == 0) {
                        push(0);
                    }
                }
                break;
            } 
```

所有的char再栈里都是int，把char当int直接计算或者强制类型转换即可，只有char作为右值的时候需要额外注意。

```
char c = 100000; //超过127
int a = c; 
```
注意，当遍历到C作为右值时，会生成一下LOD指令
```
            case CodeType::LOD :{//注意int x = a[i];这种情况a会调用LOD
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo = getVarInfo(*strPtr);
                int value;
                if (varInfo.isArray) {
                    int skew = (pop());
                     value = programStack->at(varInfo.stackIndex + skew);
                } else {
                    value = programStack->at(varInfo.stackIndex);
                }
                if (varInfo.isChar) {
                    int intValue = (value);
                    char charValue = (char)intValue;
                    push(charValue);
                } else {
                    push(value);
                }
                break;
            }
```
这样能防止把100000赋值给a

还有就是for循环的实现。
```
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
```
其中的跳转，以及其它处理都很容易出错，建议自习研究。

还有短路求值和逻辑运算。


逻辑运算中只有0和非0的区别，也就是不是0的都可以认为是1.
**不要使用&和|运算符来像处理加减乘除那样处理逻辑运算**。 
不信你可以试试 1 & 4,1 & 100.
注意

- if(1 || fun())
- if(0 && fun())

这两个例子中需要短路求值跳转，也就是说**fun()函数不能执行！**



