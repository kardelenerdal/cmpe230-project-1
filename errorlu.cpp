#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <locale>
#include <cstring>
#include <sstream>
using namespace std;

int nofTempVariables = 1;
int nofConditions = 1;
int nofLines = 0;

string line;
string outputFileName = "";
set<string> variables;
ifstream infile;            
ofstream outfile;
fstream afterAllocation;

int preced(char ch);
string expression(bool equal, string expr);

string inToPost(string infix) {
  
   stack<char> stk;
   stk.push('#');               //add some extra character to avoid underflow
   string postfix = "";         //initially the postfix string is empty
   string::iterator it;

   for(it = infix.begin(); it!=infix.end(); it++) {
      if(*it == ' '){
          postfix += " ";
          continue;
      }
      if(isalnum(char(*it)) || *it == '%'){
         postfix += *it;      //add to postfix when character is letter or number
      }else if(*it == '(')
         stk.push('(');
      else if(*it == '^')
         stk.push('^');
      else if(*it == ')') {
         while(stk.top() != '#' && stk.top() != '(') {
              postfix += " ";
            postfix += stk.top(); //store and pop until ( has found
            stk.pop();
         }
         stk.pop();          //remove the '(' from stack
      }else {
         if(preced(*it) > preced(stk.top()))
            stk.push(*it); //push if precedence is high
         else {
            while(stk.top() != '#' && preced(*it) <= preced(stk.top())) {
              postfix += " ";
               postfix += stk.top();        //store and pop until higher precedence is found
               stk.pop();
            }
            stk.push(*it);
         }
      }
   }

   while(stk.top() != '#') {
      postfix += stk.top();        //store and pop until stack is not empty.
      postfix += " ";    
      stk.pop();
   }
   return postfix;
}

int preced(char ch) {
   if(ch == '+' || ch == '-') {
      return 1;              
   }else if(ch == '*' || ch == '/') {
      return 2;            
   }else if(ch == '^') {
      return 3;            
   }else {
      return 0;
   }
}

bool isOperator(string str){
  if(str == "+" || str == "-" || str == "/" || str == "*") {
    return true;
  }
  return false;
}

string spaceCheck(string str){
    int len = str.length();
    for(int i = 0; i < len; i++){
        std::stringstream ss;
        std::string target;
        char mychar = str[i];
        ss << mychar;
        ss >> target;

        if(isOperator(target) || target == "="){
            str.insert(i+1, " ");
            str.insert(i, " ");
            i += 2;
            len += 2;
        }
    }
    return str; 
}

void error(){
  
  remove(outputFileName.c_str());
  outfile.close();
  outfile.open(outputFileName.c_str());
  outfile << "; ModuleID = 'mylang2ir'" << endl;
  outfile << "declare i32 @printf(i8*, ...)" << endl;
  outfile << "@print.error = constant [24 x i8] c\"Line %d\\0A\\00 : syntax error\"" << endl;
  outfile << "define i32 @main() {" << endl;
  outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([24 x i8]* @print.error, i32 0, i32 0), i32 "<< nofLines << " )"<< endl;
  outfile << "ret i32 0" << endl;
  outfile << "}" << endl;
  exit(0);

}

vector<string> chooseParser(string a){
  int len = 0;
    int realFirstIndex = 0;
    int realLastIndex = 0;
    int nofParenthes = 1;
    for(int i = 0; i < a.length(); i++){
       if(string(1, a[i]) == "c" && string(1, a[i+1]) == "h" && string(1, a[i+2]) == "o" && string(1, a[i+3]) == "o" && string(1, a[i+4]) == "s" && string(1, a[i+5]) == "e"){
            realFirstIndex = i;
            for(int j = i+7; j < a.length(); j++){
                if(string(1, a[j]) == "("){
                    nofParenthes++;
                }else if(string(1, a[j]) == ")"){
                    nofParenthes--;
                }
                if(nofParenthes == 0){
                    realLastIndex = j;
                    break;
                }
            }
            break;
       }
    }
    len = realLastIndex - realFirstIndex + 1;
    a = a.substr(realFirstIndex, len);    // strigi sadece chooselu kısıma çeviriyor
    int insertPoint = a.find_last_of(")");
    a.insert(insertPoint, ",");
    vector<string> parList;
    
    bool firstTime = true;
    nofParenthes = 0;
    int virgulIndex = 0;
    int lastIndex = 0;
    int firstIndex = 0;
    len = 0;
    for(int i = 0; i < a.length(); i++){
      if(string(1, a[i]) == "(" && firstTime == true){
           firstTime = false;
           firstIndex = i + 1;
           continue;
       }
       if(string(1, a[i]) == "(" && firstTime == false){
           nofParenthes++;
       } 
      if(string(1, a[i]) == ")" && firstTime == false){
           nofParenthes--;
       }
      if(string(1, a[i]) == "," && nofParenthes == 0 && firstTime == false){
        virgulIndex = i;
        lastIndex = virgulIndex - 1;
        len = lastIndex - firstIndex + 1;
        parList.push_back(a.substr(firstIndex, len));
        firstIndex = virgulIndex + 1;
      }
    }
   // for(int i = 0; i < parList.size(); i++){
    //    std::cout << parList[i] << std::endl;
   // }
    if(parList.size() != 4){
      //outfile << "choose list" << endl;
       cout <<  "errork " << endl;
      error();
    }
    return parList;
}

string choose(string line){
  vector<string> parameters = chooseParser(line);
      string par1Name = expression(false, parameters[0]);
      string firstConditionName = "%t" + to_string(nofTempVariables);
      afterAllocation << firstConditionName << " = icmp eq i32 " << par1Name << ", 0" << endl;
      nofTempVariables++;
      
      string par2Name = expression(false, parameters[1]);
      string secondConditionName = "%t" + to_string(nofTempVariables);
      afterAllocation << secondConditionName << " = icmp sgt i32 " << par1Name << ", 0" << endl;
      nofTempVariables++;
      
      string par3Name = expression(false, parameters[2]);
      string par4Name = expression(false, parameters[3]);
      string elseName = "%t" + to_string(nofTempVariables);
      afterAllocation << elseName << " = select i1 " << secondConditionName << ", i32 " << par3Name << ", i32 " << par4Name << endl;
      nofTempVariables++;
      string chooseName = "%t" + to_string(nofTempVariables);
      afterAllocation << chooseName << " = select i1 " << firstConditionName << ", i32 " << par2Name << ", i32 " << elseName << endl;
      nofTempVariables++;

      return chooseName;
  }


std::vector<std::string> expressionParser(bool equal, string line){  
  while(line.find("choose") != string::npos){
    string chooseName = choose(line);
    int chooseFirstIndex = line.find("choose");
    int chooseLastIndex = 0;
    bool inChoose = false;
    int nofParantheses = 0;
    for(int i=0; i<line.length(); i++){
      if(i == chooseFirstIndex){
        inChoose = true;
      }
      if(inChoose){
        if(line[i] == '('){
          nofParantheses++;
        } else if(line[i] == ')'){
          nofParantheses--;
          if(nofParantheses == 0){
            chooseLastIndex = i;
            break;
          }
        }
      }
    }
    string result = line.substr(0, chooseFirstIndex) + " " + chooseName + " " + line.substr(chooseLastIndex+1);
    line = result;
  }

    line = spaceCheck(line);
    int l = line.length();
    char str[l+1];
    strcpy(str, line.c_str());

    char delim[] = " ";
    char *token = strtok(str, delim);

    string leftSide = "";
    string rightSide = "";
    int leftSideCounter = 0;
    while (token) {
        if(strcmp(token, "=") == 0){
            equal = true;
        }else if(equal == false){
            if(leftSideCounter != 0){
              cout << "errorleft" <<endl;
              error();                    // a b = abc
            }
            leftSide = token;
            leftSideCounter++; 
        }else if(equal == true && token != " "){
            rightSide += token;
            rightSide += " ";
        }
        token = strtok(NULL,delim);
    }
    
    string postfix = inToPost(rightSide);
    std::stringstream test(postfix);    // postfix'i space karakterinden sonra parçalamak için kullanılan değişkenler.
    std::string segment;
    std::vector<std::string> seglist;   
  
    seglist.push_back(leftSide);      // leftSide değişkenini return edeceğimiz vector'ün 0. indexine koyduk. 
                 //  handleVariable() (bu işi expression da halledelim?)  
    while(std::getline(test, segment, ' '))  // vector'ün içine postfixteki(rightSide) elemanları atıyor.
    {
        if(segment != ""){
            seglist.push_back(segment);
        }        
    }
    return seglist;
}

void condition(string expres){
  afterAllocation << "br label %" << "cond" << nofConditions << endl;
  afterAllocation << "cond" << nofConditions << ":" << endl;
  string conditionTemp = expression(false, expres);
  afterAllocation << "%t" << nofTempVariables << " = icmp ne i32 " << conditionTemp  << ", 0" << endl;        // %t2 = icmp ne i32 %t1, 0
  afterAllocation << "br i1 %t" << nofTempVariables << ", label %body" << nofConditions << ", label %end" << nofConditions << endl;  
  nofTempVariables++; 
  afterAllocation << "body" << nofConditions << ":" << endl;   //whbody:
}

bool exists(string str){
  if(variables.find(str) == variables.end()){
    return false;
  }
  return true;
}

bool isNumber(string str){
  for(int i=0; i<str.length(); i++){
    if(str[i] > '9' || str[i] < '0'){
      return false;
    }
  }
  return true;
}

bool isVariable(string str){
  if(!isalpha(str[0])){
    return false;
  }
  for(int i=1; i<str.length(); i++){
    if(!isalnum(str[i])){
      return false;
    }
  }
  return true;
}

void storeVariable(string target, string value){
  afterAllocation << "store i32 " << value << ", i32* " << target << endl;
}
void allocateVariable(string s) {
  variables.insert(s);
  outfile << s << " = alloca i32" << endl;
  outfile << "store i32 " << 0 << ", i32* " << s << endl;
}
string loadVariable(string s){
  string tempVariableName = "%t" + to_string(nofTempVariables);
  afterAllocation << tempVariableName << " = load i32* " << s << endl;
  nofTempVariables++;
  return tempVariableName;
}



void assignment(string leftName, string value){
    
  string leftVariableName = "%" + leftName;
  if(!exists(leftVariableName)) {
    allocateVariable(leftVariableName);
  } 
  storeVariable(leftVariableName, value); 
}

string handleVariable(string var){
  string vartemp = "";
  if(var[0] == '%' || isNumber(var)){
      vartemp = var;
  }else if(isVariable(var)){
    if(exists("%"+var)){
      vartemp = loadVariable("%"+var);
    }else {
      allocateVariable("%"+var);
      vartemp = loadVariable("%"+var);
    }
  } else {
  // outfile << "iki operator" << endl;
   cout <<  "errorf " << endl;
    error(); // iki operator gelme durumu
  }
    return vartemp;
}

string expression(bool equal, string expr) {
  vector<string> postfixVector = expressionParser(!equal, expr);
  stack<string> postfixVersion;
  stack<string> waitList;
  for(int i=postfixVector.size()-1; i>0; i--) {   // ilk indexi almadım çünkü o left variable
      postfixVersion.push(postfixVector[i]);
  }
    if(postfixVersion.size() == 1) {     // expression sadece 1 variablesa
      string token = postfixVersion.top();
      postfixVersion.pop();
      string result = handleVariable(token);
      // assignment
      if (equal) {
        assignment(postfixVector[0], result);
      }
      return result;
    }
   
    while(!postfixVersion.empty()){
      string token = postfixVersion.top();
      postfixVersion.pop();
      if(isVariable(token) || isNumber(token) || token[0] == '%'){
        waitList.push(token);

      } else if(isOperator(token)){
        string var1 = waitList.top();
        string var1temp = "";
        waitList.pop();
        string var2 = waitList.top();
        string var2temp = ""; 
        waitList.pop();

        var1temp = handleVariable(var1);
        var2temp = handleVariable(var2);
        
        if(token == "+"){
          string resultName = "%t" + to_string(nofTempVariables);            //nofTempVar nerde increment oluyor?
          afterAllocation << resultName << " = add i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);

        } else if(token == "-"){

          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = sub i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);
        
        } else if(token == "/"){
          
          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = sdiv i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);

        } else if(token == "*"){

          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = mul i32 " << var1temp << ", " << var2temp << endl;
          waitList.push(resultName);
        }
        
        nofTempVariables++;
      
      }
    }
    
    if (equal) {
      string result = handleVariable(waitList.top());
      assignment(postfixVector[0], result);
    }

    return waitList.top();
}

void printStatement(string s){
  string variableName = expression(false, s);
  afterAllocation << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " << variableName << " )" << endl;
}

string fixLine(string line){
   if(line.find("#") != string::npos){
      int commentIndex = line.find("#");
      line = line.substr(0,commentIndex);
    }

  int nofParanthesis = 0;
  for(int i = 0 ; i < line.length(); i++){
    if(string(1, line[i]) == "("){
      nofParanthesis++;
    }else if(string(1, line[i]) == ")"){
      nofParanthesis--;
    }
  }
  if(nofParanthesis != 0){
    //outfile << "parantez sayısı" << endl;
    cout <<  "errore " << endl;
    error();
  }

  int startOfLine = 0;
  int endOfLine = 0;
  for (int i=0; i<line.length(); i++) {
        if (!isspace(line[i])){
          break;
        } 
        startOfLine++;
  }
   for (int i=line.length()-1; i>=0; i--) {
        if (!isspace(line[i])){
          endOfLine = i;
          break;
        }
        //endOfLine = i;
  }
  //cout << endOfLine << " asdad"<< startOfLine;
  return line.substr(startOfLine, endOfLine - startOfLine + 1);
}

int main(int argc, char const *argv[]) {

  bool isCondition = false;
  bool isIf = false;  
  
  // input file
  infile.open(argv[1]);
  
  // output file
  string inputFileName = argv[1];
  int positionOfDot = inputFileName.length();
  for(int i=0; i < inputFileName.length(); i++){
    if(inputFileName[i] == '.'){
      positionOfDot = i;
      break;
    }
  }
  outputFileName = inputFileName.substr(0, positionOfDot) + ".ll";
  outfile.open(outputFileName);
  
  // temp file
  afterAllocation.open("a.txt", ios::in | ios::out| ios::trunc);

  outfile << "; ModuleID = 'mylang2ir'" << endl;
  outfile << "declare i32 @printf(i8*, ...)" << endl;
  outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
  outfile << "define i32 @main() {" << endl;

  while(getline(infile, line)) {
    
    line = fixLine(line);
  
    if(line.find("print")!= string::npos) { // içinde statement ya da choose varsa hesapla yoksa direkt yaz 

      int startIndex = line.find("print") + 6;
      int endIndex = line.find_last_of(")");
      string exprToPrint = line.substr(startIndex, endIndex - startIndex);
      printStatement(exprToPrint);

    } else if(line.find("while")!= string::npos && isCondition == false) {  //cond da statement hesapla
      
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
      condition(exprToCheck);  
      isCondition = true;
    
    } else if(line.find("if")!= string::npos && isCondition == false) {  //cond da statement hesapla 
      
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
      condition(exprToCheck); 
      isIf = true;
      isCondition = true;

    } else if(line.find('=')!= string::npos) {   // sağı hesapla sola koy
      
      expression(true, line);  

    }else if(line.find("}") != string::npos && isCondition == true){
      
      if(!isIf){
        afterAllocation << "br label %cond" << nofConditions << endl; //br label %whcond
      } else {
        afterAllocation << "br label %end" << nofConditions << endl;
      }
      afterAllocation << "end" << nofConditions << ":" << endl;     //whend:
      isCondition = false;  
      isIf = false;
      nofConditions++;
    }else if(line.find("}") != string::npos && isCondition == false){          // arka arkaya iki } için
      //outfile << "}, false" << endl;
       cout <<  "errorb " << endl;
      error();
      
    }else if(line.find("{") != string::npos && isCondition == true){           // nested if-while için
      //outfile << "{,true" << endl;
       cout <<  "errorc " << endl;
      error();
    }else if (!line.empty()){
       cout <<  "errord " << endl;
      error(); 
      
    }                        
                                                    // zaten en son else diyeceğiz. yukarıdaki errorlere gerek var mı?
    nofLines++;
  }
 
  if(isCondition){
    nofLines--;
    cout <<  "errora " << endl;
     error();
  }

  string x;
  afterAllocation << "ret i32 0" << endl;
  afterAllocation << "}" << endl;
  afterAllocation.seekg(0, ios::beg);
  
  while(getline(afterAllocation, x)){
    outfile << x << endl;
  }
  
  afterAllocation.clear();
  afterAllocation.close();
  remove("a.txt");
}
