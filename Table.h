#pragma once
#ifndef TABLE_H
#define TABLE_H
#include<memory.h>
#include<vector>

using namespace std;
#define SIZE 109

class Table
{
private:
	vector<char> tableName;//50
	char attrNum;//1
	vector<char> primaryKey;//50
	vector<char> record;
public:
	Table(){}
	Table(const vector<char>& tName, const char& aNum, const vector<char>& pKey);
	~Table();
	 void getTableRecord(vector<char>& record);

	const int recordSize=SIZE;
};
#endif //TABLE_H

