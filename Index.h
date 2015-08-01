#pragma once
#ifndef INDEX_H
#define INDEX_H
#include<memory.h>
#include<vector>

using namespace std;
#define SIZE 158

class Index
{
private:
	vector<char> tableName;
	vector<char> attrName;
	vector<char> indexName;
public:
	Index();
	~Index();
	Index(const vector<char>& tName, const vector<char>& aName, const vector<char>& iName);
	void getIndexRecord(vector<char>& record);
	const int recordSize = SIZE;
};
#endif

