#pragma once
#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include <string>  
#include <vector>  
#include <iostream>
#include <sstream>
#include "BufferManager.h"
#include "Address.h"
#include "Table.h"
#include "Attribute.h"
#include "Index.h"
#include "RecordManager.h"
#define NAMELENGTH 50

using namespace std;

void sToVchar(const string& sName, vector<char>& vcName);

class CatalogManager
{
private:
	RecordManager& rmanager;
	BufferManager& bmanager;
public:

	CatalogManager(BufferManager& bm, RecordManager& rm) :bmanager(bm), rmanager(rm){}
	~CatalogManager(){}
	
	int isExistTable(const string& tableName);
	int isExistAttr(const string& tableName, const string& attrName);
	int isExistIndex(const string &indexName);//参数还要加？——不用！因为drop只要一个indexname
	
	int addIndex(const vector<char> &indexName, const vector<char>& tableName, const vector<char>& attrName);
	int deleteIndex(const vector<char> &indexName);

	int addTable(const vector<char>& tableName, const char& attrNum, const vector<char> &primaryKey, const vector<vector<char>>& attr, const vector<char>type, const vector<char>isUnique);
	int deleteTable(const vector<char> &tableName);

	int getAttrNum(const string &tableName);
	void getAttrInfo(const string &tableName, vector<string>& attrName, vector<int>& type, vector<int>& isKey);

	void clearFile(const string fileName);
};

#endif//CATALOGMANAGER_H

