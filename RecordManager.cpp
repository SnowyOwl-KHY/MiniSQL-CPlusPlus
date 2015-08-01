#include "RecordManager.h"

//辅助函数
void printvc(const vector<char>& vc){
	for (int i = 0; i < vc.size(); i++)
		if (vc[i])
			cout << vc[i];
		else
			cout << "0";
	cout << endl;
}

void intToChar4(int n, vector<char>& c){
	char temp[4];
	c.clear();
	*(int*)temp = n;
	for (int i = 0; i<4; i++){
		c.push_back(temp[i]);
	}
}

int char4ToInt(const vector<char>& c){
	char temp[4];
	for (int i = 0; i<4; i++){
		temp[i] = c[i];
	}
	return *(int*)temp;
}

void floatToChar4(float n, vector<char>& c){
	char temp[4];
	c.clear();
	*(float*)temp = n;
	for (int i = 0; i<4; i++){
		c.push_back(temp[i]);
	}
}

float char4ToFloat(const vector<char>& c){
	char temp[4];
	for (int i = 0; i<4; i++){
		temp[i] = c[i];
	}
	return *(float*)temp;
}

void newHead(vector<char>& headRecord, int size){
	vector<char> temp(size, 0); 
	headRecord = temp;
}

void setNextAddr(const vector<char>& record, vector<char>& nextBlock, vector<char>& nextOffset, int begin){
	nextBlock.clear();
	nextOffset.clear();
	for (int i = begin; i < begin + 4; i++){
		nextBlock.push_back(record[i]);
	}
	for (int i = begin + 4; i < begin + 8; i++){
		nextOffset.push_back(record[i]);
	}
}

void updateRecordPoint(vector<char>& record, const vector<char>& block, const vector<char>& offset, int begin){
	record.erase(record.begin() + begin, record.begin() + begin + 8);
	record.insert(record.begin() + begin, block.begin(), block.end());
	record.insert(record.begin() + begin + 4, offset.begin(), offset.end());
}

void incSize(vector<char>& head){
	vector<char> temp;
	for (int i = 0; i < 4; i++)
		temp.push_back(head[i]);
	int n=char4ToInt(temp);
	n++;
	intToChar4(n, temp);
	for (int i = 0; i < 4; i++)
		head[i] = temp[i];
}

void decSize(vector<char>& head, int m){
	vector<char> temp;
	for (int i = 0; i < 4; i++)
		temp.push_back(head[i]);
	int n = char4ToInt(temp);
	n -= m;
	intToChar4(n, temp);
	for (int i = 0; i < 4; i++)
		head[i] = temp[i];
}

int getSize(const vector<char>& head){
	vector<char> temp;
	for (int i = 0; i < 4; i++)
		temp.push_back(head[i]);
	return char4ToInt(temp);
}

Address RecordManager::findLastRecord(string fileName, const vector<char>& key, int recordSize, int begin){
	Address addr(fileName, 0, 0);
	Address nullAddr;//空地址，没找到

	if (bmanager.getFileBlockNumber(fileName) == 0){
		return nullAddr;
	}
		
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;

	vector<char> headRecord = bmanager.read(addr, headSize);//√
	vector<char> tempRecord;

	vector<char> nextBlock;
	vector<char> nextOffset;

	setNextAddr(headRecord, nextBlock, nextOffset, headSize - 8);
	int size = getSize(headRecord);//√
	int find = 0;
	int lastBlock = 0;
	int lastOffset = 0;
	for (int i = 0; i < size; i++){	
		addr.setAddr(fileName, char4ToInt(nextBlock), char4ToInt(nextOffset));//√
		tempRecord = bmanager.read(addr, recordSize);

		//以下输出不要删 有提速空间 减少查找次数！！
		/*cout <<fileName<< " "<<i << " : ";
		printvc(tempRecord);
		printvc(key);
		cout << tempRecord[begin] << tempRecord[begin+1] << endl;*/
		//判断是否相同
		for (int i = 0; i < key.size(); i++){
			find = 1;
			if (key[i] != tempRecord[i+begin]){
				find = 0;
				break;
			}
		}
		if (find)	{
			addr.setAddr(fileName, lastBlock, lastOffset);
			return addr;
		}
		lastBlock = char4ToInt(nextBlock);
		lastOffset = char4ToInt(nextOffset);
		//下一条地址
		setNextAddr(tempRecord, nextBlock, nextOffset, recordSize - 8);
	}
	return nullAddr;
}
	
Address RecordManager::insertRecord(vector<char>& newRecord, string fileName, int recordSize){
	Address addr(fileName, 0, 0);
	Address head(fileName, 0, 0);//head
	vector<char> headRecord;

	vector<char> nextBlock;
	vector<char> nextOffset;

	int newBlock;
	int newOffset;
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;

	///////文件中无记录///////
	if (bmanager.getFileBlockNumber(fileName) == 0){
		bmanager.createFile(fileName);
		
		//清空文件头
		newHead(headRecord, headSize);

		//新纪录位置(0,1)
		newBlock = 0;
		newOffset = headSize;

		//新纪录指向head（不用改了，default）
	}
	else{
		headRecord = bmanager.read(head, headSize);
		int size = getSize(headRecord);
		if (size == 0){
			//清空文件头
			newHead(headRecord, headSize);

			//新纪录位置(0,1)
			newBlock = 0;
			newOffset = headSize;

			//新纪录指向head（不用改了，default）
		}
		///////文件中有记录///////
		else{
			//新纪录位置
			setNextAddr(headRecord, nextBlock, nextOffset, headSize- 16);//第一条被删的记录
			int delBlock = char4ToInt(nextBlock);
			int delOffset = char4ToInt(nextOffset);
			
			if (delBlock == 0 && delOffset == 0){//没有被删记录
				int n;//一块中的记录数
				int block = bmanager.getFileBlockNumber(fileName) - 1;
				int offset;
				int size = getSize(headRecord);//文件中的记录数（不包括head）
				if (block == 0){
					n = (4096 - headSize) / recordSize;//第一块中的record数
					if (size == n){
						newBlock = block + 1;
						newOffset = 0;
					}
					else{//size<n
						newBlock = block;
						newOffset = headSize + (size%n)*recordSize;
					}
				}
				else{
					n = 4096 / recordSize;//其他块中最多能放的record数
					if (size == (4096 - headSize) / recordSize+block*n){
						newBlock = block + 1;
						newOffset = 0;
					}
					else{
						newBlock = block;
						newOffset = ((size - (4096 - headSize) / recordSize) % n)*recordSize;
					}
				}
			}
			else{///有被删记录
				//空位在(delBlock,delOffset)
				newBlock = delBlock;
				newOffset = delOffset;
				//修改文件头中的del指针，使它指向第二条被删记录
				addr.setAddr(fileName, delBlock, delOffset);
				vector<char> tempRecord = bmanager.read(addr, recordSize);
				setNextAddr(tempRecord, nextBlock, nextOffset, recordSize - 8);
				updateRecordPoint(headRecord, nextBlock, nextOffset, headSize - 16);
			}
			
			//newRecord指向第一条记录
			setNextAddr(headRecord, nextBlock, nextOffset, headSize - 8); 
			updateRecordPoint(newRecord, nextBlock, nextOffset, recordSize - 8);
		}
	}
	
	//插入新记录
	
	addr.setAddr(fileName, newBlock, newOffset);
	bmanager.write(addr, newRecord);//√
	//更新head
	//head指向第一条记录
	intToChar4(newBlock, nextBlock);
	intToChar4(newOffset, nextOffset);
	updateRecordPoint(headRecord, nextBlock, nextOffset, headSize - 8);
	//更新head中的size
	incSize(headRecord);
	//写回headRecord
	bmanager.write(head, headRecord);//√

	return addr;
}

int RecordManager::deleteRecord(const vector<char>& key, string fileName, int recordSize, int n){
	//cout << "deleterecord begin{" << endl;
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> delBlock;
	vector<char> delOffset;
	/*vector<char> delEndBlock;
	vector<char> delEndOffset;*/

	vector<char> headRecord;
	vector<char> lastRecord;
	vector<char> delRecord;
	vector<char> delEndRecord;

	Address head;
	Address lastAddr;
	Address delAddr;

	int attrNum=0;
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;
	head.setAddr(fileName, 0, 0);
	headRecord = bmanager.read(head, headSize);

	//找到待删记录的上一条记录
	lastAddr = findLastRecord(fileName, key, recordSize,0); 
	if (lastAddr.isNullAddr()){//找不到此记录
		//cout << "isnull!" << endl;
		return 0;
	}
	int lastRecordSize = 0;

	if (lastAddr.isHead()){//如果lastRecord为head 那么recordSize有问题
		//cout << "is head!" << endl;
		lastRecordSize = headSize;
		lastRecord = headRecord;
	}
	else{
		lastRecordSize = recordSize;
		lastRecord = bmanager.read(lastAddr, lastRecordSize);
	}
	//printvc(lastRecord);
	//cout << "lastrecordsize: " << lastRecordSize << endl;
	//得到待删的记录
	setNextAddr(lastRecord, nextBlock, nextOffset, lastRecordSize - 8);
	delBlock = nextBlock;
	delOffset = nextOffset;
	delAddr.setAddr(fileName, char4ToInt(delBlock), char4ToInt(delOffset));
	delRecord = bmanager.read(delAddr, recordSize);
	//printvc(delRecord);
	if (delRecord.size()>50)
		attrNum = delRecord[50];//获得此表的attr数，只有在delete table时用到
	
	delEndRecord = delRecord;
	Address delEndAddr = delAddr;
	for (int i = 0; i < n - 1; i++){//删除连续的n条
		setNextAddr(delEndRecord, nextBlock, nextOffset, recordSize - 8);
		//delEndBlock = nextBlock;
		//delEndOffset = nextOffset;
		delEndAddr.setAddr(fileName, char4ToInt(nextBlock), char4ToInt(nextOffset));
		delEndRecord = bmanager.read(delAddr, recordSize);
	}
	//将del上一条记录指向delEnd的下一条记录
	setNextAddr(delEndRecord, nextBlock, nextOffset, recordSize - 8);
	if (lastAddr.isHead()){
		updateRecordPoint(headRecord, nextBlock, nextOffset, headSize - 8);
	}
	else{
		updateRecordPoint(lastRecord, nextBlock, nextOffset, lastRecordSize - 8);
		bmanager.write(lastAddr, lastRecord);
	}
	
	//修改delete链
	setNextAddr(headRecord, nextBlock, nextOffset, headSize - 16);//第一条被删记录
	updateRecordPoint(delEndRecord, nextBlock, nextOffset, recordSize - 8);//将delEndRecord指向第一条被删记录
	bmanager.write(delEndAddr, delEndRecord);//update过的都要写回！！
	updateRecordPoint(headRecord, delBlock, delOffset, headSize - 16);//head指向delRecord
	decSize(headRecord, n);
	bmanager.write(head,headRecord);

	//cout << "}deleterecord end 1" << endl;
	return attrNum;
}

template<class T>
int compare(T x, T y){
	//cout << "compare: " << x <<" "<<y<< endl;
	if (x == y)
		return 0;
	else if (x < y)
		return 1;
	else
		return 2;
}

int compareVc(const vector<char>& value, vector<char> record, int begin, int end, int type){
	vector<char> temp;
	temp.insert(temp.begin(), record.begin() + begin, record.begin() + end);
	if (type == 1){//int
		return compare(char4ToInt(temp), char4ToInt(value));
	}
	else if (type == 2){//float
		return compare(char4ToFloat(temp), char4ToFloat(value));
	}
	else{//char(n)
		string st = "";
		string sv = "";
		for (int i = 0; i < temp.size(); i++){
			st += temp[i];
		}
		for (int i = 0; i < value.size(); i++){
			sv += value[i];
		}
		return compare(st, sv);
	}
}

vector<Address> RecordManager::findAllRecord(string fileName, const vector<int>& op, const vector<vector<char>>& value,
	const vector<int>& begin, const vector<int>& end, const vector<int> &type, int recordSize, 
	vector<vector<char>>& returnRecord, int toDelete){
	vector<Address> returnAddr;
	vector<Address> nullAddr;
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> tempRecord;
	Address addr;

	if (bmanager.getFileBlockNumber(fileName) == 0){
		return nullAddr;
	}

	Address head(fileName, 0, 0);
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;
	vector<char> headRecord = bmanager.read(head, headSize);
	setNextAddr(headRecord, nextBlock, nextOffset, headSize - 8);
	int size = getSize(headRecord);
	//cout << "findallrecord: size: " << size << endl;

	for (int i = 0; i < size; i++){
		addr.setAddr(fileName, char4ToInt(nextBlock), char4ToInt(nextOffset));
		tempRecord = bmanager.read(addr,recordSize);
		int flag = 0;
		for (int j = 0; j < op.size(); j++){//取一条记录i和j个条件比
			int n=compareVc(value[j], tempRecord, begin[j], end[j], type[j]);
			if (n == 0 && (op[j] == 0 || op[j] == 3 || op[j] == 4))
				continue;
			else if (n == 1 && (op[j] == 1 || op[j] == 3 || op[j] == 5))
				continue;
			else if (n == 2 && (op[j] == 2 || op[j] == 4 || op[j] == 5))
				continue;
			else{
				flag = 1;
				break;
			}
		}
		//cout <<"flag: "<< flag << endl;
		if (flag==0){
			if (toDelete){
				//printvc(tempRecord);
				deleteRecord(tempRecord, fileName, recordSize, 1);
			}
			returnRecord.push_back(tempRecord);
			returnAddr.push_back(addr);
		}
		setNextAddr(tempRecord, nextBlock, nextOffset, recordSize - 8);
	}
	return returnAddr;
}

vector<Address> RecordManager::getAllRecord(string fileName, int recordSize, vector<vector<char>>& returnRecord){
	vector<Address> returnAddr;
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> tempRecord;
	Address addr;

	if (bmanager.getFileBlockNumber(fileName) == 0)
		return returnAddr;

	Address head(fileName, 0, 0);
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;
	vector<char> headRecord = bmanager.read(head, headSize);
	setNextAddr(headRecord, nextBlock, nextOffset, headSize - 8);
	int size = getSize(headRecord);
	//cout << "size: " << size << endl;
	//cout << recordSize << endl;
	for (int i = 0; i < size; i++){
		//cout << char4ToInt(nextBlock) << " "<<char4ToInt(nextOffset) << endl;
		addr.setAddr(fileName, char4ToInt(nextBlock), char4ToInt(nextOffset));
		tempRecord = bmanager.read(addr, recordSize);
		returnRecord.push_back(tempRecord);
		returnAddr.push_back(addr);
		//printvc(tempRecord);
		setNextAddr(tempRecord, nextBlock, nextOffset, recordSize - 8);
	}
	return returnAddr;
}

int RecordManager::deleteAll(string fileName, int recordSize){
	vector<Address> returnAddr;
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> tempRecord;
	Address addr;

	Address head(fileName, 0, 0);
	int headSize;
	if (recordSize < 20)
		headSize = 20;
	else
		headSize = recordSize;
	//cout << fileName << " " << headSize << endl;
	vector<char> headRecord = bmanager.read(head, headSize);
	setNextAddr(headRecord, nextBlock, nextOffset, headSize - 8);
	int size = getSize(headRecord);

	bmanager.clearFile(fileName);
	//cout << size <<endl;
	return size;
}

vector<char> RecordManager::getNextRecord(Address addr, string fileName, int recordSize){
	vector<char> nextBlock;
	vector<char> nextOffset;
	vector<char> r=bmanager.read(addr, recordSize);
	setNextAddr(r, nextBlock, nextOffset, recordSize - 8);
	addr.setAddr(fileName, char4ToInt(nextBlock), char4ToInt(nextOffset));
	return bmanager.read(addr, recordSize);
}

vector<char> RecordManager::getRecord(Address addr, int recordSize){
	return bmanager.read(addr, recordSize);
}