#pragma once
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include<vector>
using namespace std;

class Attribute
{
private:
	vector<char> tableName;
	vector<char> attrName;
	char type;
	char isUnique;
public:
	Attribute(){}
	Attribute(const vector<char>& tName, const vector<char>& aName, const char t, const char isU);
	~Attribute();
	void setAttrRecord(const vector<char>& tName, const vector<char>& aName, const char t, const char isU);
	void getAttrRecord(vector<char>& record);
	const int recordSize = 110;
};

#endif//TTRIBUTE_H

