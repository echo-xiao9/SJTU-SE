#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->clearButton,SIGNAL(clicked()),this,SLOT(clearAll()));
    connect(ui->codeLineEdit, SIGNAL(returnPressed()), this, SLOT(on_codeLineEdit_return()));
    connect(ui->loadButton,SIGNAL(clicked()),this,SLOT(on_loadButton_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateCodeBrowser(){
    ui->codeBrowser->clear();
    QString appendLine="";
    auto iter = statements.begin();
    while (iter !=statements.end()) {
        QString singleStmt = iter->second->stmt;
        appendLine =QString(QString::number(iter->first))+' '+ iter->second->stmt;
        ++iter;
        ui->codeBrowser->append(appendLine);
    }
}

void MainWindow::updateResultBrowser(QString s){
    ui->codeBrowser->append(s);
}


void MainWindow::on_codeLineEdit_return(){
    QString input = ui->codeLineEdit ->text();
    ui->codeLineEdit->clear();
    ui->codeBrowser->clear();
    //input is a statement
    parse_line(input);
    updateCodeBrowser();
}

void MainWindow::clearAll(){
    ui->codeLineEdit->clear();
    ui->codeBrowser->clear();
    ui->resultBrowser->clear();
    ui->syntaxDisplayBroser->clear();
    statements.clear();
    variables.clear();
}

void MainWindow::on_loadButton_clicked(){
    loadStat();
    updateCodeBrowser();
}

int MainWindow ::stmtNum(stmt_t Stmt){
    for(int i=0;i<8;i++){
        if(Stmt ==stmtTab[i]) return i;
    }
    return -1;
}

int MainWindow ::cmdNum(cmd_t Cmd){
    for(int i=0;i<6;i++){
        if(Cmd ==cmdTab[i]) return i;
    }
    return -1;
}

parse_t MainWindow:: parse_line(QString &line){
    stmt_t stmtTmp;
    cmd_t cmdTmp;
    QString lineTmp=line;
    QString varName="";
    QString exp="";
    QString exp1="";
    QString delim="";
    vector<Token> expInputVec;
    vector<Token> expInputVec1;
    expInputVec.clear();
    expInputVec1.clear();
    pair<map<int, Statement*>::iterator, bool> Insert_Pair;
    Statement *newStmt;
    int lineNum=0;
    int numTmp=0;
    // is command
    if(IS_NUM(line)){ 
        if(parse_num(lineTmp,lineNum)==PARSE_ERR||
                parse_stmt(lineTmp, stmtTmp)==PARSE_ERR)
            return PARSE_ERR;
        switch (stmtNum(stmtTmp))
        {
        case 0: //"INPUT": 35 INPUT n3
          if(parse_var(lineTmp, varName) == PARSE_ERR) return PARSE_ERR;
              ui->codeLineEdit-> setText("?");
              numTmp = ui->codeLineEdit ->text().toInt();
//              newStmt = new InputStmt(lineNum, varName, numTmp, statements);
              newStmt = new InputStmt(lineNum, varName,numTmp, variables);
          break;
        case 1://"LET": //40 LET total = n1 + n2 + n3
          if(parse_var(lineTmp, varName)==PARSE_ERR || 
              parse_delim(lineTmp, delim)!=PARSE_CON||
              delim !="="||
              parse_exp(lineTmp,expInputVec)==PARSE_ERR)
              return PARSE_ERR;
              newStmt=new LetStmt(lineNum, varName, exp, variables);
          break;

        case 2: //"GOTO" GOTO n
          if(parse_num(lineTmp,numTmp)==PARSE_ERR)return PARSE_ERR;
          newStmt = new GotoStmt(lineNum, numTmp,variables);
          break;

        case 3://"IF":  //IF condition THEN n

          if(parse_exp(lineTmp, expInputVec)==PARSE_ERR ||
          parse_exp(lineTmp,expInputVec)==PARSE_ERR||
          parse_delim(lineTmp, delim)!=PARSE_CON||
          parse_exp(lineTmp,expInputVec1)==PARSE_ERR||
          parse_stmt(lineTmp, stmtTmp)==PARSE_ERR||
          stmtTmp !="THEN"||
          parse_num(lineTmp, numTmp)==PARSE_ERR
          )return PARSE_ERR;
          newStmt = new IfStmt(lineNum, exp,  delim, exp1, numTmp,variables);

          break;

        case 4: //"PRINT": //PRINT 2 + 2
          if(parse_exp(lineTmp, expInputVec)==PARSE_ERR)return PARSE_ERR;
          newStmt = new PrintStmt(lineNum,exp, variables);
          break;

        case 5: //"REM": //REM aaa
          newStmt = new RemStmt(lineNum,lineTmp,variables);
          break;

        case 6: //"END":

          break;

        default:
          break;
        }
           Insert_Pair = statements.insert(pair<int, Statement*>(lineNum, newStmt));
            // if the index exits
            if(!Insert_Pair.second) {
                auto iter = statements.find(lineNum);
                 // if we can find the same index, replace it with the new one.
                if(iter != statements.end()) statements.erase(iter);
                Insert_Pair = statements.insert(pair<int, Statement*>(lineNum, newStmt));
            }
    }
      else if(IS_LETTER(line)){
        if(parse_cmd(lineTmp, cmdTmp)==PARSE_ERR)return PARSE_ERR;
        switch (cmdNum(cmdTmp))
        {
        case 0://"RUN":
          
          break;
        case 1://"LOAD":
          loadStat();
          break;
        case 2://"LIST":
          
          break;
        case 3: //"CLEAR":
        clearAll();
          
          break;
        case 4: //"HELP":
          
          break;
        case 5: //"QUIT":
          QApplication::quit();
          break;
        default:
          break;
        }
      }



}
parse_t MainWindow:: parse_stmt(QString &ptr, stmt_t& stmt){
    QString tmp=ptr;
    tmp = tmp.trimmed();
    int index=-1; int i=0;
    if(IS_END(tmp)) return PARSE_ERR;
    for(i=0;i<8;i++){
        index = tmp.indexOf(stmtTab[i], 0,  Qt::CaseInsensitive);
        if(index != -1) break;
    }
    if(index == 0) {
        stmt = stmtTab[i];
        ptr = ptr.mid(stmt.length()+1);
        return PARSE_STMT;
    }
    return PARSE_ERR;
}

parse_t MainWindow:: parse_cmd(QString &ptr, stmt_t& cmd){
    QString tmp=ptr;
    tmp = tmp.trimmed();
    int index=-1; int i=0;
    if(IS_END(tmp)) return PARSE_ERR;
    for(i=0;i<6;i++){
        index = tmp.indexOf(cmdTab[i], 0,  Qt::CaseInsensitive);
        if(index != -1) break;
    }

    if(index == 0) {
        cmd = cmdTab[i];
        ptr = tmp.mid(cmd.length());
        return PARSE_CMD;
    }
    return PARSE_ERR;
}

parse_t MainWindow:: parse_num(QString &ptr, int & val){
    QString tmp=ptr;
    int i=0;
    tmp=tmp.trimmed();
    if(IS_END(tmp)) return PARSE_ERR;
    if(!IS_NUM(tmp)) return PARSE_ERR;
    for(i=0;i<tmp.length()&&(tmp[i]>='0' && tmp[i]<='9');i++){
    }
    val=tmp.mid(0,i).toInt();
    ptr=tmp.mid(i);
    return PARSE_NUM;
}


parse_t MainWindow:: parse_var(QString &ptr, QString& name){
//    begin with a letter or an underscore.  only have letters, numbers,  underscore
    QString tmp=ptr;
    int i=0;
    tmp=tmp.trimmed();
    if(IS_END(tmp)) return PARSE_ERR;
    if(!(tmp[0] == '_'|| IS_LETTER(tmp)))return PARSE_ERR;
    while(tmp[i]=='_' ||(tmp[i]>='0'&& tmp[i]<='9') ||
          (tmp[i]>='a'&& tmp[i]<='z') || (tmp[i]>='A'&& tmp[i]<='Z')){
        i++;
    }
    name = tmp.mid(0,i);
    tmp = tmp.mid(i);
    ptr = tmp;
    if(IS_END(name))return PARSE_ERR;
    return PARSE_VAR;
}

parse_t MainWindow:: parse_exp(QString &ptr, vector<Token> &expInputVec){
    // the if case needs extra consideration.
    // if a>b then 12 the if case is the only case in which the expression does not appear at the end
    int index=-1;
    int num=0;
    QString tmp ;
    QString oper;
    QString expParseTab[]={"THEN", ">", "=","<" };
    QString var="";
    for(int i=0; i<4; i++){
        index =ptr.indexOf(expParseTab[i] ,0 , Qt::CaseInsensitive);
        if(index>0) {
            tmp = ptr.mid(0, index);
            ptr = ptr.mid(index);
            break; //break the for?
        }
    }
    if(index==-1) tmp=ptr;
    tmp= tmp.trimmed();
    if(IS_END(tmp))return PARSE_ERR;
    while (!IS_END(tmp)) {
        Token token;
        if(IS_NUM(tmp)){
            if(parse_num(tmp, num)==PARSE_NUM) return PARSE_ERR;
            token.type=NUM;
            token.value = num;
        }
        else if(IS_LETTER(tmp)){
            if(parse_var(tmp, var) == PARSE_ERR) return PARSE_ERR;
            token.type = VAR;
            token.name = var;
        }
        else if(IS_OPERATOR(tmp)){
            if(parse_delim(tmp, oper) !=PARSE_OP)return PARSE_ERR;
            for(int i=0;i<7;i++){
                if(tmp == opTab[i]) token.type = optokenTab[i];
            }
        }else{return PARSE_ERR;}
          expInputVec.push_back(token);
    }


    return PARSE_EXP;
}

parse_t MainWindow:: parse_delim(QString &ptr, QString& delim){
     QString tmp=ptr;
     tmp=tmp.trimmed();
     if(IS_END(tmp)) return PARSE_ERR;
     if(tmp[0]=='=' || tmp[0] =='>' || tmp[0] == '<'){
         delim =tmp.mid(0,1);
         tmp = tmp.mid(1);
         ptr = tmp;
         return PARSE_CON;
     }
     if(tmp[0]=='+' || tmp[0] =='-' || tmp[0] == '/'|| tmp[0] == '(' ||tmp[0] == ')'){
         delim =tmp.mid(0,1);
         tmp = tmp.mid(1);
         ptr = tmp;
         return PARSE_OP;
     }
     if(tmp[0] == '*'){
         if(tmp[1]=='*') {
             delim = tmp.mid(0,2);
             tmp = tmp.mid(2);
         }
         else {
             delim = tmp.mid(0,1);
             tmp = tmp.mid(1);
         }
         ptr = tmp;
         return PARSE_OP;
     }
     return PARSE_ERR;
}



stmt_t *  MainWindow::find_instr(QString name){}

bool MainWindow::judge_infix(string str)
{
    int temp=0;
    if(str[0]=='/'||str[0]=='*')
        return false;
    if(str[str.length()-1]<'0'&&str[str.length()-1]>'9' )
        return false;
    for(int i=0;i<str.length();i++)
    {
        if(str[i]=='(')
        {
            if(i==0&&(str[i+1]=='*'||str[i+1]=='/'))
                return false;
            else if(str[i-1]>='0'&&str[i-1]<='9' )
                return false;
            temp++;
        }
        else if(str[i]==')')
        {
            if(i==0)
                return false;
            else if(str[i-1]=='+'||str[i-1]=='*'||str[i-1]=='-'||str[i-1]=='/')
                return false;
            else if(str[i+1]>='0'&&str[i+1]<='9')
                return false;
            temp--;
        }
    }
    if(temp==0)
        return true;
            return false;
        }




void MainWindow::loadStat(){
    QString fileName = QFileDialog::getOpenFileName();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"can't open file"<<endl;
         return;
    }
    statements.clear();//clear current statements before loading
    QTextStream in(&file);
    QString line ="";
    while (!in.atEnd()) {
            QString line = in.readLine();
            parse_line(line);
        }
    file.close();
}
