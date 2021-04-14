#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <locale>
using namespace std;

int nofTempVariables = 1;
string line;
set<string> variables;

void assigment(string line){
	
	int equalSign = line.find('=');
	string left = line.substr(0, equalSign);
	string right = line.substr(equalSign+1);
		
	// yoksa allocate et 
	string leftVariableName = "%" + left;
	if(exists(leftVariableName)) {
		allocateVariable(leftVariableName);
		variables.push_back(leftVariableName);
	} 
	// sağ tarafı hesapla
	string rightVariableName = expression(right);
		
	// sola koy (store et)
	storeVariable(leftVariableName, rightVariableName);	

}
void allocateVariable(string s) {
	outfile << s << " = alloca i32" << endl;
	storeVariable(s, "0");
}
void storeVariable(string target, string value){
	outfile << "store i32 " << value << ", i32* " << target << endl;
}
string loadVariable(string s){
	string tempVariableName = "%t" + to_String(nofTempVariables)
	outfile << tempVariableName << " = load i32* " << s << endl;
	return tempVariableName;
}
void printStatement(string s){
	// islemler
	string variableName = expression(s);
	outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "<< variableName <<" )"<< endl;
}

chooseFunction();
whileStatement();
ifStatement();

// variable varsa load et yoksa allocate et numbersa ya da tempse bişey yapma
string handleVariable(string var){
	string vartemp = "";
	if(var[0] == '%' || isNumber(var)){
   			vartemp = var;
   	} else if(isVariable(var)){
   		// daha once varsa load et yoksa allocate et
	   	if(exists(var)){
	   			vartemp = loadVariable("%"+var);
	   	} else {
	   		allocateVariable("%"+var);
			vartemp = loadVariable("%"+var)
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
   	while(!postfixVersion.isEmpty()){

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

   				string resultName = "%t" + nofTempVariables;
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
	if(!str[0].isalpha()){
		return false;
	}
	for(int i=1; i<str.length(); i++){
		if(!str[i].isalnum()){
			return false;
		}
	}
	return true;
}

int main(int argc, char const *argv[]) {
	
	ifstream infile;                            
	infile.open(argv[1]);
	ofstream outfile;
	outfile.open(argv[2]);

	while(getline(infile, line)) {


		// one line or multiple line
		
		// one line
		if(line.find("print")!= string::npos) { // içinde statement ya da choose varsa hesapla yoksa direkt yaz 

			string exprToPrint = line.substr(line.find("print")+6, line.length()-1);
			printStatement(exprToPrint);

		} else if(line.find('while')!= string::npos) {  //cond da statement hesapla

		} else if(line.find('if')!= string::npos) {  //cond da statement hesapla 

		} else if(line.find('=')!= string::npos) {   // sağı hesapla sola koy
			assigment(line);
		} else if(line.find('choose')!= string::npos) {   // içinde statementlar var onları hesapla3 tane if koy

		}

	}
}
