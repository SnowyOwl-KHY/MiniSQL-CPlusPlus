#define _CRT_SECURE_NO_WARNINGS
#include "Interpreter.h"

int main()
{
	Interpreter interpreter;
	string instr;

	while (1){
		cout << "minisql> ";

		instr=interpreter.readIn();
		vector<string> element=interpreter.separate(instr);
		if (interpreter.execute(element) == 0)//quit
			break;

		cout << endl;
	}

	getchar();
	return 0;
}