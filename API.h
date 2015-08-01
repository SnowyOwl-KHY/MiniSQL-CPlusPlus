#pragma once
#ifndef API_H
#define API_H
#include <string>  
#include <vector>  
#include <iostream>
#include <memory.h>
#include <time.h>

#include "CatalogManager.h"
#include "RecordManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

using namespace std;

class API
{
private:
	CatalogManager& cmanager;
	RecordManager& rmanager;
	IndexManager& imanager;
public:
	~API() {}
	API(CatalogManager& cm,RecordManager& rm, IndexManager& im): cmanager(cm),rmanager(rm), imanager(im) {}
	int createTable(const string &tableName, const vector<string> &attr, const vector<int> &type, const vector<int> &isKey);
	int dropTable(const string tableName);
	int createIndex(const string tableName, const string attrName, const string indexName, const vector<int>& type, int n);
	int dropIndex(const string indexName);
	
	int insert(const string tableName, const vector<string> &value,const vector<int>& type);
	int select(const string tableName,  const vector<string> &attr, const vector<string> &op, const vector<string> &value,
		const vector<int>& type, const vector<int>& begin, int recordSize, vector<vector<char>>& returnRecord, int toDelete);

	void showTables();
};

template<class T>
vector<T> subVector(const vector<T>& v, int begin, int end){
	vector<T> temp;
	for (int i = begin; i < end; i++){
		temp.push_back(v[i]);
	}
	return temp;
}
string put_(int n);
string putbx(int n);
void drawTable(const vector<vector<string>>& table, vector<int>& columnWide);
void describeTable(const vector<string> &attr, const vector<int> &type, const vector<int> &isKey);
int showTable(const vector<vector<char>>& record, const vector<int>& type, const vector<string>& attr_show);

#endif //API_H
