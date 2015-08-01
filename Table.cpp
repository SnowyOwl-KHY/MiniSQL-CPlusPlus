#include "Table.h"

Table::Table(const vector<char>& tName, const char& aNum, const vector<char>& pKey){
	tableName.insert(tableName.end(), tName.begin(), tName.end());
	attrNum = aNum;
	primaryKey.insert(primaryKey.end(), pKey.begin(), pKey.end()); 
}

Table::~Table()
{
}

void Table::getTableRecord(vector<char>& record){
	record.clear();
	record.insert(record.end(), tableName.begin(), tableName.end());
	record.push_back(attrNum);
	record.insert(record.end(), primaryKey.begin(), primaryKey.end());
	for (int i = 0; i < 8; i++){
		record.push_back(0);
	}
}




