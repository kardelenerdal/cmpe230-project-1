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

string line;
set<string> variables;
ifstream infile;            
ofstream outfile;
fstream afterAllocation;

int preced(char ch);
string expression(bool equal, string expr);

string inToPost(string infix) {
  // cout <<"infix" <<infix <<endl;
   stack<char> stk;
   stk.push('#');               //add some extra character to avoid underflow
   string postfix = "";         //initially the postfix string is empty
   string::iterator it;

   for(it = infix.begin(); it!=infix.end(); it++) {
      if(*it == ' '){
          postfix += " ";
          continue;
      }
      if(isalnum(char(*it))){
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
   //cout <<"postfix "<< postfix << endl;
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

// line = "a = choose(3,((4)),(a-5),6) + 6 + choose(0,1,2,3)"
// parantez sayısı 0 iken ve ) gördüğüm zaman kapanma indexi
// line = "a = %t8 + 6 + choose(0,1,2,3)"
// parametleri çözüp çözülen parametlerden biri choose ise onu at choose'a sonra onu koy 
// choose'u çağıran satırı yazdır

// line = " choose(choose(523), choose(), a, b) "
// line = "choose("%t6", choose(), a,b)"
// recline= "choose(523)" "%t6"
string chooseFinder(string line){
   
}

// // choose(0,1,2,3) + 5
std::vector<std::string> expressionParser(bool equal, string line){  
   // while choose varsa
   // choose fonksiyonuna line'ı at
   // line bana ilk choose u halletmiş bir şekilde yeni line'ı vercek
   
   // sonuç olarak choose suz yeni bir line'ım var 
   // line = "a = choose(3,4,(a-5),6) + 6 + choose(0,1,2,3)"
   // line = "a = %t8 + 6 + choose(0,1,2,3)"
   // line = "a = %t8 + 6 + %t50"
    line = spaceCheck(line);
    
    int l = line.length();
    char str[l+1];
    strcpy(str, line.c_str());

    char delim[] = " ";
    char *token = strtok(str, delim);
    //bool equal = false; equal variable'ını parametre olarak aldık, eğer printse bu değer true olarak gelecek ve sol tarafı hesaplamamamıza gerek kalmayacak. Diğer türlü durumda false gelecek.
   // cout <<"equal "<<equal<<endl;
    string leftSide = "";
    string rightSide = "";
    while (token) {
      //cout << token<< " token"<< endl;
        if(strcmp(token, "=") == 0){
            equal = true;
        }else if(equal == false){
            leftSide = token; 
        }else if(equal == true && token != " "){
         // cout << token <<" token"<< endl;
            rightSide += token;
            rightSide += " ";
           // cout << rightSide <<" right side"<< endl;
        }
        token = strtok(NULL,delim);
    }
   // cout << "ls "<< leftSide <<endl;
    //cout  << rightSide << endl;
    
    string postfix = inToPost(rightSide);
    //cout << postfix << endl; 
    //cout << postfix << endl;
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

// choose(0,1,2,3) + 5
void condition(string expres){
  // nofTempVar ? 
  afterAllocation << "br label %" << "cond" << nofConditions << endl;
  afterAllocation << "cond" << nofConditions << ":" << endl;
  string conditionTemp = expression(false, expres);
   // conditiontemp = %t7
  afterAllocation << "%t" << nofTempVariables << " = icmp ne i32 " << conditionTemp  << ", 0" << endl;        // %t2 = icmp ne i32 %t1, 0
  afterAllocation << "br i1 %t" << nofTempVariables << ", label %body" << nofConditions << ", label %end" << nofConditions << endl;  
  nofTempVariables++; 
  //br i1 %t2, label %whbody, label %whend
  afterAllocation << "body" << nofConditions << ":" << endl;   //whbody:
}

// bu variable daha once tanımlanmış mı
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

// bu functiondan cok emin değilim
bool isVariable(string str){
  // first character is always a letter
  // the rest can be letter or number
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
    
  // yoksa allocate et varsa bir şey yapmıyoruz
  string leftVariableName = "%" + leftName;
  if(!exists(leftVariableName)) {
    allocateVariable(leftVariableName);
  } 
  // sola koy (store et)
 // cout << "storelama"<< value <<endl;
  storeVariable(leftVariableName, value); 

}

// variable varsa load et yoksa allocate et numbersa ya da tempse bişey yapma
string handleVariable(string var){
  string vartemp = "";
  if(var[0] == '%' || isNumber(var)){
        vartemp = var;
    } else if(isVariable(var)){
      // daha once varsa load et yoksa allocate et
      if(exists("%"+var)){
        //cout << "loadlama"<< var << endl;
          vartemp = loadVariable("%"+var);
      } else {
        allocateVariable("%"+var);
        vartemp = loadVariable("%"+var);
      }
    }
    return vartemp;
}

// choose(0,1,2,3) + 5
string expression(bool equal, string expr) {
  // equal true ise gelen line'da = var ve assignment yapıyor, false ise = yok
  // expr = a+2*x/(5-z)+(2/y)
  vector<string> postfixVector = expressionParser(!equal, expr);
  stack<string> postfixVersion;
  stack<string> waitList;
   // %t6 + 5
  // postfixe çevirme
  for(int i=postfixVector.size()-1; i>0; i--) {   // ilk indexi almadım çünkü o left variable
      postfixVersion.push(postfixVector[i]);
  }
  // postfixVersion = a2x*5z-/+2y/+  a'dan başlarayak çıkıcak.
  
    if(postfixVersion.size() == 1) {     // expression sadece 1 variablesa
      string token = postfixVersion.top();
      postfixVersion.pop();
      string result = handleVariable(token);
      //cout << "token" <<result <<endl;
      // assignment
      if (equal) {
        assignment(postfixVector[0], result);
      }
      return result;
    }
   
    while(!postfixVersion.empty()){

      string token = postfixVersion.top();
      postfixVersion.pop();

      if(isVariable(token) || isNumber(token)){
      //cout << "variable"<<token<<endl;
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
    
    // assignment yapıyor
    if (equal) {
      string result = handleVariable(waitList.top());
      assignment(postfixVector[0], result);
      //cout << "token" <<waitList.top() <<endl;
    }

    return waitList.top();
}

void printStatement(string s){
  // islemler
  string variableName = expression(false, s);
  afterAllocation << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " << variableName << " )" << endl;
}

int main(int argc, char const *argv[]) {

  bool isCondition = false;
  bool isIf = false;  
  
  infile.open(argv[1]);
  outfile.open(argv[2]);
  afterAllocation.open("a.txt", ios::in | ios::out| ios::trunc);

  outfile << "; ModuleID = 'mylang2ir'" << endl;
  outfile << "declare i32 @printf(i8*, ...)" << endl;
  outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
   // choose bloğu
   // noftemp = 25 buna bak
  outfile << "define i32 @main() {" << endl;

  while(getline(infile, line)) {

   // comment
    if(line.find("#") != string::npos){
      int commentIndex = line.find("#");
      line = line.substr(0,commentIndex);
    }
    
  
    if(line.find("print")!= string::npos) { // içinde statement ya da choose varsa hesapla yoksa direkt yaz 

      int startIndex = line.find("print") + 6;
      int endIndex = line.find_last_of(")");
      string exprToPrint = line.substr(startIndex, endIndex - startIndex);
      //cout << exprToPrint << endl;
      printStatement(exprToPrint);

    } else if(line.find("while")!= string::npos && isCondition == false) {  //cond da statement hesapla
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
      condition(exprToCheck);  
      isCondition = true;
    
    } else if(line.find("if")!= string::npos && isCondition == false) {  //cond da statement hesapla 
      // if( choose(0,1,2,3) + 5)
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
       // choose(0,1,2,3) + 5
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
    }

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
}
