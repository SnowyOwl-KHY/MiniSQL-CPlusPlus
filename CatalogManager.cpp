#include "CatalogManager.h"

void sToVchar(const string& sName, vector<char>& vcName){//长度为50
	vcName.clear();
	for (int i = 0; i < NAMELENGTH; i++){
		if (i < sName.size())
			vcName.push_back(sName[i]);
		else
			vcName.push_back(0);
	}
}

int CatalogManager::isExistTable(const string& tableName){
	vector<char> tName;
	sToVchar(tableName, tName);
	Table table;
	Address addr = rmanager.findLastRecord("table.catalog", tName, table.recordSize,0); 
	
	if (addr.isNullAddr())
		return 0;
	else
		return 1;
}

int CatalogManager::isExistAttr(const string& tableName, const string& attrName){
	//若存在，返回attr的序号+1
	vector<char> tName;
	sToVchar(tableName, tName);
	Attribute attr;
	Address addr = rmanager.findLastRecord("attr.catalog", tName, attr.recordSize,0);
	if (addr.isNullAddr())
		return 0;
	
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> record = bmanager.read(addr, attr.recordSize);
	int n = 0;
	int attrNum = getAttrNum(tableName);
	
	int find=0;
	vector<char> aName;
	for (int i = 0; i < attrNum; i++){
		setNextAddr(record, nextBlock, nextOffset, attr.recordSize - 8);
		addr.setAddr("attr.catalog", char4ToInt(nextBlock), char4ToInt(nextOffset));
		record = bmanager.read(addr, attr.recordSize);
		find = 1;
		sToVchar(attrName, aName);
		for (int j = 0; j < 50; j++){
			if (record[j + 50] != aName[j]){
				find = 0;
				break;
			}
		}
		if (find){
			n = i;
			break;
		}
	}
	
	if (find == 0)
		return 0;
	
	return n+1;
}

int CatalogManager::isExistIndex(const string &indexName){
	vector<char> iName;
	sToVchar(indexName, iName);
	Index index;
	Address addr = rmanager.findLastRecord("index.catalog", iName, index.recordSize,0);
	if (addr.isNullAddr())
		return 0;
	else
		return 1;

}

int CatalogManager::addTable(const vector<char>& tableName, const char& attrNum, const vector<char>& primaryKey, const vector<vector<char>>& attr, const vector<char>type, const vector<char>isUnique){
	vector<char> newRecord;
	
	//管理table.catalog文件
	Table tableRecord(tableName, attrNum, primaryKey);
	tableRecord.getTableRecord(newRecord);
	
	rmanager.insertRecord(newRecord, "table.catalog", tableRecord.recordSize);
	
	//管理tableName文件
	string temp = "";
	for (int i = 0; i < tableName.size(); i++)
		temp += tableName[i];
	bmanager.createFile(temp);

	//管理attr.catalog文件
	Attribute attrRecord;
	for (int i = attrNum-1; i >= 0 ; i--){//因为是从头开始插，所以要倒着插
		attrRecord.setAttrRecord(tableName, attr[i], type[i], isUnique[i]);
		attrRecord.getAttrRecord(newRecord);
		rmanager.insertRecord(newRecord, "attr.catalog", attrRecord.recordSize);
	}

	return 1;
}

int CatalogManager::deleteTable(const vector<char> &tableName){
	//修改table.catalog
	Table table;
	int attrNum = rmanager.deleteRecord(tableName, "table.catalog", table.recordSize, 1);

	//删除tableName
	string fn = "";
	for (int i = 0; i < tableName.size(); i++)
		fn += tableName[i];
	bmanager.deleteFile(fn);

	//修改attr.catalog
	Attribute attr;
	rmanager.deleteRecord(tableName, "attr.catalog", attr.recordSize, attrNum);
	
	//修改index
	//构建参数
	vector<vector<char>> returnRecord;
	
	vector<int> op; op.push_back(0);
	vector<vector<char>> value; value.push_back(tableName);
	vector<int> begin; begin.push_back(50);
	vector<int> end; end.push_back(100);
	vector<int> type; type.push_back(53);
	Index index;
	rmanager.findAllRecord("index.catalog", op, value, begin, end, type, index.recordSize, returnRecord, 1);
	for (int i = 0; i < returnRecord.size(); i++){
		string temp = "";
		for (int j = 0; j < 50; j++)
			if (returnRecord[i][j])
				temp += returnRecord[i][j];
		bmanager.deleteFile(temp+".index");
	}

	return 1;
}

int CatalogManager::addIndex(const vector<char> &indexName, const vector<char>& tableName, const vector<char>& attrName){
	//管理index.catalog文件
	vector<char> newRecord;
	Index indexRecord(tableName,attrName,indexName);
	indexRecord.getIndexRecord(newRecord);
	//cout << "insert index :" << endl;
	//printvc(newRecord);
	rmanager.insertRecord(newRecord, "index.catalog", indexRecord.recordSize);

	/*string iName="";
	for (int i = 0; i < indexName.size(); i++)
		if (indexName[i])
			iName += indexName[i];
	bmanager.createFile(iName);*/
	return 1;
}

int CatalogManager::deleteIndex(const vector<char> &indexName){
	Index index;
	rmanager.deleteRecord(indexName, "index.catalog", index.recordSize, 1);

	return 1;
}

int CatalogManager::getAttrNum(const string &tableName){
	vector<char> tName;
	sToVchar(tableName, tName);

	vector<char> nextBlock;
	vector<char> nextOffset;
	Address addr;
	Table table;
	
	addr = rmanager.findLastRecord("table.catalog", tName, table.recordSize,0);
	if (addr.isNullAddr())
		return 0;
	vector<char> r = bmanager.read(addr, table.recordSize);
	setNextAddr(r, nextBlock, nextOffset, table.recordSize - 8);
	addr.setAddr("table.catalog", char4ToInt(nextBlock), char4ToInt(nextOffset));
	r = bmanager.read(addr, table.recordSize);
	
	return r[50];
}

void CatalogManager::getAttrInfo(const string &tableName, vector<string>& attrName, vector<int>& type, vector<int>& isKey){
	vector<char> tName;
	sToVchar(tableName, tName);

	Attribute attr;
	vector<char> nextBlock;
	vector<char> nextOffset;
	Address addr = rmanager.findLastRecord("attr.catalog", tName, attr.recordSize,0);
	vector<char> r=bmanager.read(addr, attr.recordSize);
	int n = getAttrNum(tableName);
	for (int i = 0; i<n; i++){
		setNextAddr(r, nextBlock, nextOffset, attr.recordSize - 8);
		addr.setAddr("attr.catalog", char4ToInt(nextBlock), char4ToInt(nextOffset));
		r = bmanager.read(addr, attr.recordSize);	
		string temp = "";
		for (int j = 50; j < 100; j++){
			if (r[j]){
				temp += r[j];
			}
			else
				break;
		}
		attrName.push_back(temp);
		type.push_back(r[100]);
		isKey.push_back(r[101]);
	}
}

void CatalogManager::clearFile(const string fileName){
	bmanager.clearFile(fileName);
}