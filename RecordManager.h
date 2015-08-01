#pragma once
#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
#include <string>  
#include <vector>  
#include <iostream>
#include <sstream>
#include "BufferManager.h"
#include "Address.h"
#include "Table.h"
#include "Attribute.h"
#include "Index.h"

using namespace std;

class RecordManager
{
private:
	BufferManager& bmanager;
public:
	RecordManager(BufferManager& bm):bmanager(bm){}
	~RecordManager(){}

	Address insertRecord(vector<char>& newRecord, string fileName, int recordSize);//返回值和地址
	int deleteRecord(const vector<char>& key, string fileName, int recordSize, int n);//返回值
	Address findLastRecord(string fileName, const vector<char>& key, int recordSize, int begin);

	vector<Address> findAllRecord(string fileName, const vector<int>& op, const vector<vector<char>>& value,
		const vector<int>& begin, const vector<int>& end, const vector<int> &type, int recordSize, 
		vector<vector<char>>& returnRecord, int toDelete);
	vector<Address> getAllRecord(string fileName, int recordSize, vector<vector<char>>& returnRecord);
	int deleteAll(string fileName, int recordSize);

	vector<char> getNextRecord(Address addr, string fileName, int recordSize);
	vector<char> getRecord(Address addr, int recordSize);
};

//辅助函数 for catalog
void printvc(const vector<char>& vc);
void intToChar4(int n, vector<char>& c);
int char4ToInt(const vector<char>& c);
void floatToChar4(float n, vector<char>& c);
float char4ToFloat(const vector<char>& c);
void newHead(vector<char>& headRecord, int size);
void setNextAddr(const vector<char>& record, vector<char>& nextBlock, vector<char>& nextOffset, int begin);
void updateRecordPoint(vector<char>& record, const vector<char>& block, const vector<char>& offset, int begin);

#endif //RECORDMANAGER_H
