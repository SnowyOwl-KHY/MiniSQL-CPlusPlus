#include "API.h"

string put_(int n){
	string s = "";
	for (int i = 0; i < n; i++)
		s += "-";
	return s;
}

string putbx(int n){
	string s = "";
	for (int i = 0; i < n; i++)
		s += " ";
	return s;
}

void drawTable(const vector<vector<string>>& table, vector<int>& columnWide){
	vector<string> row;
	/*for (int i = 0; i < columnWide.size(); i++){
	cout << columnWide[i] << endl;
	}*/
	string a;
	for (int j = 0; j < columnWide.size(); j++){
		a += "+-" + put_(columnWide[j]) + "-";
	}
	a += "+";

	string t;
	int n = table[0].size();
	int m = table.size();
	row.push_back(a);
	for (int i = 0; i < n; i++){//行
		t = "";
		if (i == 1)
			row.push_back(a);
		for (int j = 0; j < m; j++){//列
			t += "| " + table[j][i] + putbx(columnWide[j] - table[j][i].size()) + " ";
		}
		t += "|";
		row.push_back(t);
	}
	row.push_back(a);

	for (int i = 0; i < row.size(); i++){
		cout << row[i] << endl;
	}
}

void describeTable(const vector<string> &attr, const vector<int> &type, const vector<int> &isKey){
	vector<vector<string>> table;
	vector<int> maxSize;
	vector<string> temp;
	int size = 0;

	temp.push_back("Field");
	size = temp[0].size();
	for (int i = 0; i < attr.size(); i++){
		temp.push_back(attr[i]);
		if (attr[i].size()>size)
			size = attr[i].size();
	}
	table.push_back(temp);
	maxSize.push_back(size);

	temp.clear();
	temp.push_back("Type");
	size = temp[0].size();
	for (int i = 0; i < type.size(); i++){
		if (type[i] == 1)
			temp.push_back("int");
		else if (type[i] == 2)
			temp.push_back("float");
		else{
			string s = "char(";
			int n = type[i] - 3;
			stringstream ss;
			string str;
			ss << n;
			ss >> str;
			s += str;
			s += ")";
			temp.push_back(s);
		}
		if (temp[1 + i].size() > size)
			size = temp[1 + i].size();
	}
	table.push_back(temp);
	maxSize.push_back(size);

	temp.clear();
	temp.push_back("Null");
	size = temp[0].size();
	for (int i = 0; i <isKey.size(); i++){
		if (isKey[i] == 2)
			temp.push_back("NO");
		else
			temp.push_back("YES");

		if (temp[1 + i].size() > size)
			size = temp[1 + i].size();
	}
	table.push_back(temp);
	maxSize.push_back(size);

	temp.clear();
	temp.push_back("Key");
	size = temp[0].size();
	for (int i = 0; i <isKey.size(); i++){
		if (isKey[i] == 2)
			temp.push_back("PRI");
		else if (isKey[i] == 1)
			temp.push_back("UNI");
		else
			temp.push_back("");
		if (temp[1 + i].size() > size)
			size = temp[1 + i].size();
	}
	table.push_back(temp);
	maxSize.push_back(size);

	temp.clear();
	temp.push_back("Default");
	size = temp[0].size();
	for (int i = 0; i < type.size(); i++){
		if (type[i] == 1)
			temp.push_back("0");
		else if (type[i] == 2)
			temp.push_back("NULL");
		else{
			temp.push_back("NULL");
		}
		if (temp[1 + i].size() > size)
			size = temp[1 + i].size();
	}
	table.push_back(temp);
	maxSize.push_back(size);

	drawTable(table, maxSize);
	cout << attr.size() << " rows in set." << endl;
}

int showTable(const vector<vector<char>>& record, const vector<int>& type, const vector<string>& attr_show){
	vector<vector<string>> table;
	vector<string> column;
	vector<int> maxSize;
	int size;
	//record确定行数，type&attr_show确定列数
	int n = 0;
	for (int i = 0; i < attr_show.size(); i++){
		column.clear();
		column.push_back(attr_show[i]);
		size = attr_show[i].size();

		for (int j = 0; j < record.size(); j++){
			string t = "";
			if (type[i] == 1){//int
				int x = char4ToInt(subVector(record[j], n, n + 4));
				stringstream ss;
				ss << x;
				t = ss.str();
			}
			else if (type[i] == 2){//float
				float x = char4ToFloat(subVector(record[j], n, n + 4));
				stringstream ss;
				ss << x;
				t = ss.str();
			}
			else{
				vector<char> vt = subVector(record[j], n, n + type[i] - 3);
				for (int k = 0; k < vt.size(); k++){
					if (vt[k])
						t += vt[k];
				}
			}

			if (t.size()>size){
				size = t.size();
			}
			column.push_back(t);
		}
		table.push_back(column);
		maxSize.push_back(size);
		if (type[i] == 1 || type[i] == 2)
			n += 4;
		else
			n += (type[i] - 3);
	}

	drawTable(table, maxSize);

	return record.size();
}

int API::createTable(const string &tableName, const vector<string> &attr, const vector<int> &type, const vector<int> &isKey){
	//tableName
	vector<char> tName;
	sToVchar(tableName, tName);
	
	//attrName
	char attrNum = (char)attr.size();
	
	//primaryKey
	string primaryKey;
	for (int i = 0; i < attr.size(); i++){
		if (isKey[i] == 2){
			primaryKey=attr[i];
			break;
		}
	}
	vector<char> pKey;
	sToVchar(primaryKey, pKey);

	//attrName s
	vector< vector<char> > aName;
	vector<char> temp;
	for (int i = 0; i < attrNum; i++){
		sToVchar(attr[i], temp);
		aName.push_back(temp);
	}

	//type & isKey
	vector<char> t;
	vector<char> isk;
	for (int i = 0; i < type.size(); i++){
		t.push_back((char)type[i]);
		if (isKey[i])
			isk.push_back(1);
		else
			isk.push_back(0);
	}

	if (cmanager.addTable(tName, attrNum, pKey, aName, t, isk))
		return 1;
	else
		return 0;

}

int API::dropTable(const string tableName){
	vector<char> tName;
	sToVchar(tableName, tName);
	if (cmanager.deleteTable(tName))
		return 1;
	else
		return 0;
}

int API::createIndex(const string tableName, const string attrName, const string indexName, const vector<int>& type,int n){
	vector<char> tName;
	vector<char> aName;
	vector<char> iName;

	sToVchar(tableName, tName);
	sToVchar(attrName, aName);
	sToVchar(indexName, iName);

	if (cmanager.addIndex(iName,tName, aName))
		;
	else
		return 0;

	//////////
	int recordSize = 0;
	int begin;
	int typex;
	for (int i = 0; i < type.size(); i++){
		if (i == n){
			begin = recordSize;
			typex = type[i];
		}
		switch (type[i]){
		case 1:
		case 2:recordSize += 4; break;
		default:recordSize += type[i] - 3;
		}
	}
	recordSize += 8;

	int keySize;
	KeyType keyType;
	switch (typex) {
	case 1: keyType = INT; keySize = 4; break;
	case 2: keyType = FLOAT; keySize = 4; break;
	default: keyType = CHAR; keySize = typex - 3; break;
	}
	imanager.createIndex(indexName, keyType, keySize);
	vector<vector<char>> returnRecord;
	vector<Address> returnAddr=rmanager.getAllRecord(tableName, recordSize, returnRecord);
	
	//截出所有记录中的那个属性值
	vector<vector<char>> values;
	vector<char> temp;
	for (int i = 0; i < returnRecord.size(); i++){
		int m;
		switch (typex) {
		case 1: 
		case 2: m = 4; break;
		default: m = type[n] - 3; break;
		}
		temp.clear();
		for (int j = 0; j < m; j++)
			temp.push_back(returnRecord[i][j + begin]);
		values.push_back(temp);
	}

	for (int i = 0; i < values.size(); i++) {
		imanager.insert(indexName, values[i], returnAddr[i]);
	}
	return 1;
}

int API::dropIndex(const string indexName){
	vector<char> iName;

	sToVchar(indexName, iName);
	if (cmanager.deleteIndex(iName))
		;
	else
		return 0;

	imanager.dropIndex(indexName);
	return 1;
}

int API::insert(const string tableName, const vector<string>& value,const vector<int>& type){
	vector<char> temp;
	vector<char> valueForBtree;
	vector<char> tName;
	int k = -1;
	sToVchar(tableName, tName);
	Index index;
	string attrHasIndex = "";
	string indexName = "";
	Address addr = rmanager.findLastRecord("index.catalog", tName, index.recordSize, 50);
	if (addr.isNullAddr())//如果这个表上没有索引
		;
	else{
		temp = rmanager.getNextRecord(addr, "index.catalog", index.recordSize);
		for (int i = 0; i < 50; i++){
			if (temp[i + 100])
				attrHasIndex += temp[i+100];
			if (temp[i])
				indexName += temp[i];
		}
		k = cmanager.isExistAttr(tableName, attrHasIndex)-1;//第几个属性有index
	}
	
	vector<char> newRecord;
	int recordSize = 0;
	for (int i = 0; i < type.size(); i++){
		if (type[i] == 1){//int
			intToChar4(atoi(value[i].c_str()), temp);
			recordSize += 4;
		}
		else if (type[i] == 2){//float
			floatToChar4(atof(value[i].c_str()), temp);
			recordSize += 4;
		}
		else{//char(n)
			temp.clear();
			int n = type[i] - 3;
			for (int j = 0; j < n; j++){
				if (j < value[i].size())//value[i]是string
					temp.push_back(value[i][j]);
				else
					temp.push_back(0);
			}
			recordSize += n;
		}
		newRecord.insert(newRecord.end(), temp.begin(), temp.end());
		if (i == k){
			valueForBtree = temp;
		}
	}

	for (int i = 0; i < 8; i++){
		newRecord.push_back(0);
	}
	recordSize += 8;

	Address returnAddr = rmanager.insertRecord(newRecord, tableName, recordSize);
	if (indexName != "")
		imanager.insert(indexName, valueForBtree, returnAddr);
	return 1;
}

int typeToInt(string op){
	if (op == "=")
		return 0;
	else if (op == "<")
		return 1;
	else if (op == ">")
		return 2;
	else if (op == "<=")
		return 3;
	else if (op == ">=")
		return 4;
	else if (op == "<>")
		return 5;
}

int API::select(const string tableName, const vector<string> &attr, const vector<string> &op, const vector<string> &value,
	const vector<int>& type, const vector<int>& begin, int recordSize, vector<vector<char>>& returnRecord, int toDelete){
	vector<int> end;
	vector<char> temp;
	vector<vector<char>> valuex;
	vector<int> opx;
	vector<Address> returnAddr;
	if (op.size()==0){//selete * from xxx;//无条件
		if (toDelete){
			//判断有无索引
			vector<char> tName;
			sToVchar(tableName, tName);
			vector<vector<char>> returnRecord;
			vector<int> op; op.push_back(0);
			vector<vector<char>> value; value.push_back(tName);
			vector<int> begin; begin.push_back(50);
			vector<int> end; end.push_back(100);
			vector<int> type; type.push_back(53);
			Index index;
			rmanager.findAllRecord("index.catalog", op, value, begin, end, type, index.recordSize, returnRecord, 0);
			for (int i = 0; i < returnRecord.size(); i++){//若有 则清空索引文件
				string temp = "";
				for (int j = 0; j < 50; j++)
					if (returnRecord[i][j])
						temp += returnRecord[i][j];
				//bmanager.deleteFile(temp + ".index");
				//cmanager.clearFile(temp + ".index");
					
					imanager.rebuildIndex(temp);
			}
			return rmanager.deleteAll(tableName, recordSize);
		}
		else{
			//cout << "okok" << endl;
			returnAddr=rmanager.getAllRecord(tableName, recordSize, returnRecord);
			//cout << "okok" << endl;
			return returnAddr.size();
		}
	}
	else{//有条件
		for (int i = 0; i < op.size(); i++){
			if (type[i] == 1){//int
				end.push_back(begin[i] + 4);
				intToChar4(atoi(value[i].c_str()), temp);
				valuex.push_back(temp);
				opx.push_back(typeToInt(op[i]));
			}
			else if (type[i] == 2){//float
				end.push_back(begin[i] + 4);
				floatToChar4(atof(value[i].c_str()), temp);
				valuex.push_back(temp);
				opx.push_back(typeToInt(op[i]));
			}
			else{//char(n)
				end.push_back(begin[i] + type[i]-3);
				temp.clear();
				for (int k = 0; k < value[i].size(); k++)
					temp.push_back(value[i][k]);
				//sToVchar(value[i], temp);
				valuex.push_back(temp);
				opx.push_back(typeToInt(op[i]));
			}
		}
		
		//判断属性是否都有索引
		vector<int> in;
		vector<string> attrHasIndex;

		Index index;
		string indexName;
		int useIndex = 1;
		vector<char> key;
		vector<char> key1;
		sToVchar(tableName, key);
		for (int i = 0; i < attr.size(); i++){
			int flag = 0;
			for (int j = 0; j < attrHasIndex.size(); j++)//查找这个属性有无在attrHasIndex中
				if (attr[i] == attrHasIndex[j]){
					flag = 1;
					break;
				}
			if (flag)
				continue;
			
			//查找这个属性上有无index
			key.erase(key.begin() + 50, key.end());
			sToVchar(attr[i], key1);
			key.insert(key.end(), key1.begin(), key1.end());
			Address addr = rmanager.findLastRecord("index.catalog", key, index.recordSize, 50);
			if (addr.isNullAddr()){//条件中有一个属性没有索引
				//cout << attr[i] << ": no index!" << endl;
				useIndex = 0;
				break;
			}
			else{//有索引
				//cout << attr[i] << ": index!" << endl;
				temp = rmanager.getNextRecord(addr, "index.catalog", index.recordSize);
				indexName = "";
				for (int i = 0; i < 50; i++){
					if (temp[i])
						indexName += temp[i];
				}
				attrHasIndex.push_back(indexName);
				in.push_back(i);
			}

		}

		//选择select方式
		if (!useIndex||attrHasIndex.size()>1){
			clock_t start = clock();
			returnAddr = rmanager.findAllRecord(tableName, opx, valuex, begin, end, type, recordSize, returnRecord, toDelete);
			clock_t finish = clock();
			printf("Time consumed: %f second\n", (double)(finish - start) / CLOCKS_PER_SEC);
		}
		else{//符合使用index加速的条件
			if (toDelete == 0){//select
				cout << "use index" << endl;
				clock_t start = clock();
				/*for (int i = 0; i < valuex.size(); i++)
					printvc(valuex[i]);*/
				returnAddr = imanager.select(indexName, valuex, opx);
				//cout << "ok" << endl;
				returnRecord.clear();
				for (int i = 0; i < returnAddr.size(); i++){
					returnAddr[i].setAddrFile(tableName);
					returnRecord.push_back(rmanager.getRecord(returnAddr[i], recordSize));
				}
				clock_t finish = clock();
				printf("Time consumed: %f second\n", (double)(finish - start) / CLOCKS_PER_SEC);

			}
			else{//delete
				returnAddr = rmanager.findAllRecord(tableName, opx, valuex, begin, end, type, recordSize, returnRecord, toDelete);

				for (int i = 0; i < attrHasIndex.size(); i++)
					for (int j = 0; j < returnRecord.size(); j++)
						imanager.deleteKey(attrHasIndex[i], subVector(returnRecord[j], begin[in[i]], end[in[i]]));				
			}
		}

		//returnAddr = rmanager.findAllRecord(tableName, opx, valuex, begin, end, type, recordSize, returnRecord, toDelete);
			
		return returnAddr.size();
	}
}

void API::showTables(){
	Table table;
	vector<vector<char>> returnRecord;
	rmanager.getAllRecord("table.catalog", table.recordSize, returnRecord);
	vector<int> type;
	type.push_back(53);
	vector<string> title;
	title.push_back("Tables_in_miniSQL");
	cout << showTable(returnRecord, type, title) << " rows in set." << endl;
}