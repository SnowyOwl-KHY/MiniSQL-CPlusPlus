#include "Index.h"
Index::Index()
{
}

Index::~Index()
{
}

Index::Index(const vector<char>& tName, const vector<char>& aName, const vector<char>& iName){
	tableName = tName;
	attrName = aName;
	indexName = iName;
}

void Index::getIndexRecord(vector<char>& record){
	record = indexName;
	record.insert(record.end(), tableName.begin(), tableName.end());
	record.insert(record.end(), attrName.begin(), attrName.end());
	for (int i = 0; i < 8; i++){
		record.push_back(0);
	}
}
