#include <iostream>
#include <fstream>
#include <vector> //sdfghjk
using namespace std;

int nofTempVariables = 1;
string line;
vector<string> variables;

void allocateVariable(string s) {
	outfile << s << " = alloca i32" << endl;
	storeVariable(s, "0");
}
void storeVariable(string target, string value){
	outfile << "store i32 " << value << ", i32* " << target << endl;
}
void loadVariable(string s){
	string tempVariableName = "%t" + to_String(nofTempVariables);
	outfile << tempVariableName << " = load i32* " << s << endl;
}
void printStatement(string s){
	// islemler
	expression(s);
	string variableName = expression(s);
	outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "<< variableName <<" )"<< endl;
}
chooseFunction();
whileStatement();
ifStatement();
string expression();

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
			 
			int equalSign = line.find('=');
			string left = line.substr(0, equalSign);
			string right = line.substr(equalSign+1);
			
			// yoksa allocate et 
			string leftVariableName = "%" + left;
			if(find(variables.begin(), variables.end(), left) == variables.end()) {
				allocateVariable(left);
				variables.push_back(leftVariableName);
			} 
			// sağ tarafı hesapla
			string rightVariableName = expression(right);
			
			// sola koy (store et)
			storeVariable(leftVariableName, rightVariableName);

		} else if(line.find('choose')!= string::npos) {   // içinde statementlar var onları hesapla3 tane if koy

		} 

	}
}
