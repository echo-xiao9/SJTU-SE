#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <QString>
#include "Exp.h"
#include <vector>
using namespace std;
extern vector<var>variables;
class Statement
{
public:
    Exp *exp = nullptr;
    Exp *exp1 =nullptr;
    int index;
    QString stmt;
    // -1 means no type the type order from 0-6 is
    // 0:INPUT   1:LET   2:GOTO  3:IF   4:PRINT    5:REM   6:END
    int type ;
    Statement(int inputIndex, int Type);
    virtual QString runSingleStmt(QString par)=0;
    virtual QString tree(int i=0)=0;
    ~Statement();
};


class InputStmt: public Statement{
public:
    QString varName;
    int varVal;
    InputStmt(int inputIndex, QString  varName1, int varVal1);
    QString runSingleStmt(QString par);
    QString findVar();
    QString tree(int i=0);
};

class LetStmt: public Statement{
private:
    QString letVarName;
    //have exp
public:
    LetStmt(int InputIndex, QString VarName, QString expr);
    QString runSingleStmt(QString par);
    QString tree(int i=0);
};

class GotoStmt: public Statement{
    int targetNum;
public:
    GotoStmt(int inputIndex, int targetLineNum);
    QString runSingleStmt(QString par);
    QString tree(int i=0);
};

class IfStmt: public Statement{
private:
    //have exp and exp1
    QString inputExp;
    QString inputExp1;
    QString condtion;
    int targetNum;

public:
     IfStmt(int inputIndex, QString  exp,  QString inputCondition, QString exp1, int targetNum);
     QString runSingleStmt(QString par);
     QString tree(int i=0){
         if(i==1)return QString::number(targetNum);
         return condtion;
     }
};


class PrintStmt: public Statement{
    QString expStr;
public:
    //have exp
    PrintStmt(int inputIndex, QString exp);
    QString runSingleStmt(QString par);
    QString tree(int i=0);
};

class RemStmt: public Statement{
    QString remark;
public:
    RemStmt(int inputIndex, QString lineTmp);
    QString runSingleStmt(QString par);
    QString tree(int i=0);
};


class EndStmt: public Statement{
public:
    EndStmt(int inputIndex);
    QString runSingleStmt(QString par);
    QString tree(int i=0);
};



#endif // STATEMENT_H
