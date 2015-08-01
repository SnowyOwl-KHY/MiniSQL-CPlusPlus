#pragma once
#ifndef INTERPRETER_H  
#define INTERPRETER_H
#include <string>  
#include <vector>  
#include <iostream>
#include <algorithm>
#include "CatalogManager.h"
#include "API.h"
#include "BufferManager.h"
#include "RecordManager.h"

using namespace std;

class Interpreter
{
private:
	BufferManager bmanager;
	CatalogManager cmanager;
	RecordManager rmanager;
	IndexManager imanager;
	API api;
	void handleError(int num, string name, string s);
	int isLegal_name(string name);
	int isLegal_type(string type);
	int isLegal_op(string op);
	int isTypeMatch(string &value, int type);
	int createTable(vector<string> &element, int size);
	int insertRecord(vector<string> &element, int size);
	int select(vector<string> &element, int size, int toDelete, int isInsert);
public:
	Interpreter():rmanager(bmanager),cmanager(bmanager,rmanager), imanager(bmanager), api(cmanager,rmanager, imanager) {}
	~Interpreter();
	string readIn();
	vector<string> separate(string instr);
	int execute(vector<string> &element);
};
#endif //INTERPRETER_H 
