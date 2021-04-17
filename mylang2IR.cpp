#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <locale>
#include <cstring>
#include <vector>
#include <sstream>
using namespace std;

int nofTempVariables = 1;
int nofConditions = 1;

string line;
set<string> variables;
ifstream infile;            
ofstream outfile;

string inToPost(string infix ) {
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
      return 1;              //Precedence of + or - is 1
   }else if(ch == '*' || ch == '/') {
      return 2;            //Precedence of * or / is 2
   }else if(ch == '^') {
      return 3;            //Precedence of ^ is 3
   }else {
      return 0;
   }
}

std::vector<std::string> expressionParser(bool equal){  

    char str[] = "a00 a11 = (d *((d2 - d3)))";
    char delim[] = " ";
    char *token = strtok(str,delim);
    //bool equal = false; equal variable'ını parametre olarak aldık, eğer printse bu değer true olarak gelecek ve sol tarafı hesaplamamamıza gerek kalmayacak. Diğer türlü durumda false gelecek.
    
    string leftSide = "";
    string rightSide = "";

    while (token)
    {
        if(strcmp(token, "=") == 0){
            equal = true;
        }else if(equal == false){
            leftSide = token; 
        }else if(equal == true){
            rightSide += token;
            rightSide += " ";
        }
        token = strtok(NULL,delim);
    }
    
    
    string postfix = inToPost(rightSide); 
    
    std::stringstream test(postfix);    // postfix'i space karakterinden sonra parçalamak için kullanılan değişkenler.
    std::string segment;
    std::vector<std::string> seglist;  	
	
    seglist.push_back(leftSide);  		// leftSide değişkenini return edeceğimiz vector'ün 0. indexine koyduk. 
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
	// nofTempVar ? 
	outfile << nofConditions << "cond:" << endl;
	string condtionTemp = expression(expres);
       	outfile << "%t" << nofTempVariables << " = icmp ne i32 " << conditionTemp  << ", 0" << endl;				// %t2 = icmp ne i32 %t1, 0
	outfile << "br i1 %t" << nofTempVariables << ", label %" << nofConditions << "body, label %" << nofConditions << "end" << endl;  
        nofTempVariables++;	
	//br i1 %t2, label %whbody, label %whend
	outfile << nofConditions << "body:" << endl;	 //whbody:
}

string expression(string expr);
// bu variable daha once tanımlanmış mı
bool exists(string str){
	if(variables.find(str) == variables.end()){
		return false;
	}
	return true;
}

bool isOperator(string str){
	if(str == "+" || str == "-" || str == "/" || str == "*") {
		return true;
	}
	return false;
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
	outfile << "store i32 " << value << ", i32* " << target << endl;
}
void allocateVariable(string s) {
	variables.insert(s);
	outfile << s << " = alloca i32" << endl;
	storeVariable(s, "0");
}
string loadVariable(string s){
	string tempVariableName = "%t" + to_string(nofTempVariables);
	outfile << tempVariableName << " = load i32* " << s << endl;
	return tempVariableName;
}



void assigment(string line){
	
	// burası böyle olmaz
	int equalSign = line.find('=');
	string left = line.substr(0, equalSign);
	string right = line.substr(equalSign+1);
		
	// yoksa allocate et 
	string leftVariableName = "%" + left;
	if(!exists(leftVariableName)) {
		allocateVariable(leftVariableName);
	} 
	// sağ tarafı hesapla
	string rightVariableName = expression(right);
		
	// sola koy (store et)
	storeVariable(leftVariableName, rightVariableName);	

}

//chooseFunction();
//whileStatement();
//ifStatement();

// variable varsa load et yoksa allocate et numbersa ya da tempse bişey yapma
string handleVariable(string var){
	string vartemp = "";
	if(var[0] == '%' || isNumber(var)){
   			vartemp = var;
   	} else if(isVariable(var)){
   		// daha once varsa load et yoksa allocate et
	   	if(exists("%"+var)){
	   			vartemp = loadVariable("%"+var);
	   	} else {
	   		allocateVariable("%"+var);
			vartemp = loadVariable("%"+var);
   		}
   	}
   	return vartemp;
}

string expression(string expr) {

	stack<string> postfixVersion;
	stack<string> waitList;
	// postfixe çevirme
	for(int i=0; i<expr.length(); i++){
			
	}
    // postfix oldu

    if(postfixVersion.size() == 1) {     // expression sadece 1 variablesa
    	string token = postfixVersion.top();
    	postfixVersion.pop();
    	string result = handleVariable(token);
    	return result;
    }
   
   	while(!postfixVersion.empty()){

   		string token = postfixVersion.top();
   		postfixVersion.pop();

   		if(isVariable(token) || isNumber(token)){
   			
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

   				string resultName = "%t" + nofTempVariables;						//nofTempVar nerde increment oluyor?
   				outfile << resultName << " = add i32 " << var2temp << ", " << var1temp;
   				waitList.push(resultName);

   			} else if(token == "-"){

   				string resultName = "%t" + nofTempVariables;
   				outfile << resultName << " = sub i32 " << var2temp << ", " << var1temp;
   				waitList.push(resultName);
   			
   			} else if(token == "/"){
   				
   				string resultName = "%t" + nofTempVariables;
   				outfile << resultName << " = udiv i32 " << var2temp << ", " << var1temp;
   				waitList.push(resultName);

   			} else if(token == "*"){
   				string resultName = "%t" + nofTempVariables;
   				outfile << resultName << " = mul i32 " << var1temp << ", " << var2temp;
   				waitList.push(resultName);
   			}

   		}
   	}
   	
   	return waitList.top();
}

void printStatement(string s){
	// islemler
	string variableName = expression(s);
	outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "<< variableName <<" )"<< endl;
}

int main(int argc, char const *argv[]) {

	bool isCondition = false;
	bool isIf = false;	
	
	infile.open(argv[1]);
	outfile.open(argv[2]);

	outfile << "; ModuleID = 'mylang2ir'" << endl;
	outfile << "declare i32 @printf(i8*, ...)" << endl;
	outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
	outfile << "define i32 @main() {" << endl;

	while(getline(infile, line)) {


		// one line or multiple line
		
		// one line
		if(line.find("print")!= string::npos) { // içinde statement ya da choose varsa hesapla yoksa direkt yaz 

			string exprToPrint = line.substr(line.find("print")+6);
			exprToPrint = exprToPrint.substr(0, exprToPrint.length()-1);
			//cout << exprToPrint << endl;
			printStatement(exprToPrint);

		} else if(line.find("while")!= string::npos && isCondition = false) {  //cond da statement hesapla
			condition();	
			isCondition = true;
		
		} else if(line.find("if")!= string::npos && isCondition = false) {  //cond da statement hesapla 
			condition();	
			isIf = true;
			isCondition = true;

		} else if(line.find('=')!= string::npos) {   // sağı hesapla sola koy
			
			assigment(line);	

		}else if(line.find("}") != string::npos && isCondition = true){
			
			if(!isIf){
				outfile << "br label %" << nofConditions << "cond" << endl;	//br label %whcond
			}
			outfile << nofConditions << "end:" << endl;			//whend:
			isCondition = false;	
			isIf = false;
			nofConditions++;
		}

	}

	outfile << "ret i32 0" << endl;
	outfile << "}" << endl;
}
