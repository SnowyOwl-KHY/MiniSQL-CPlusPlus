#include "IndexManager.h"
using namespace std;
using namespace BPlusTree;

const char* indexPostfix = ".index";
const char* tablePostfix = ".table";

extern Compare compares[4];

IndexManager::IndexManager(BufferManager& bufferManager) : mBufferManager(bufferManager) {

}

IndexManager::~IndexManager() {

}


bool IndexManager::createIndex(string index, KeyType keyType, int keySize) {
	string fileName = index + indexPostfix;
	mBufferManager.createFile(fileName);
	TreeHeader treeHeader;	//添加初始文件头
	treeHeader.mRoot = 1;
	treeHeader.mKeyType = keyType;
	treeHeader.mKeySize = keySize;
	char tempData[treeHeaderSize];
	*(TreeHeader*)tempData = treeHeader;
	vector<char> blockData(blockSize, '\0');
	for (int i = 0; i < treeHeaderSize; i++) {
		blockData[i] = tempData[i];
	}
	int fileOffset = mBufferManager.appendBlock(fileName, blockData);
	assert(fileOffset == 0);

	NodeHeader nodeHeader;	//添加初始根节点头
	nodeHeader.mIsUsed = true;
	nodeHeader.mIsLeaf = true;
	nodeHeader.mFather = NULLPOINTER;
	nodeHeader.mKeyNumber = 0;
	char tempNodeHeader[nodeHeaderSize];
	*(NodeHeader*)tempNodeHeader = nodeHeader;
	for (int i = 0; i < nodeHeaderSize; i++) {
		blockData[i] = tempNodeHeader[i];
	}
	Pointer pointer = NULLPOINTER;	//初始的指向下一个叶节点的指针
	char tempPointer[pointerSize];
	*(Pointer*)tempPointer = pointer;
	for (int i = 0; i < pointerSize; i++) {
		blockData[nodeHeaderSize + i] = tempPointer[i];
	}

	fileOffset = mBufferManager.appendBlock(fileName, blockData);
	assert(fileOffset == 1);

	return true;
}

bool IndexManager::dropIndex(string index) {
	return mBufferManager.deleteFile(index + indexPostfix);
}

bool IndexManager::rebuildIndex(string index) {
	TreeHeader treeHeader = getTreeHeader(index);
	dropIndex(index);
	createIndex(index, treeHeader.mKeyType, treeHeader.mKeySize);
	return true;
}

vector<Address> IndexManager::select(string index, const vector<Key>& valuex, const vector<int>& opx) {
	bool beginEqualFlag = true;
	bool endEqualFlag = true;
	vector<Key> notEqual;
	vector<Address> notEqualAddress;
	Key begin;
	Key end;
	TreeHeader treeHeader = getTreeHeader(index);
	Compare com = compares[treeHeader.mKeyType];
	vector<Address> result;
	for (int i = 0; i < valuex.size(); i++) {
		switch (opx[i]) {
		case 0:
			try {
				result.push_back(select(index, valuex[i]));
			}
			catch (NoSuchElementException e) {
				//do nothing
			}
			return result;
		case 1://op = "<"
			if (end.size() == 0 || com(valuex[i], end) == true) {
				end = valuex[i];
				endEqualFlag = false;
			}
			break;
		case 2://op = ">"
			if (begin.size() == 0 || com(begin, valuex[i]) == true) {
				begin = valuex[i];
				beginEqualFlag = false;
			}
			break;
		case 3://op = "<="
			if (end.size() == 0 || com(valuex[i], end) == true) {
				end = valuex[i];
				endEqualFlag = true;
			}
			break;
		case 4://op = ">="
			if (begin.size() == 0 || com(begin, valuex[i]) == true) {
				begin = valuex[i];
				beginEqualFlag = true;
			}
			break;
		case 5://op = "<>"
			notEqual.push_back(valuex[i]);
			break;
		}
	}


	result = select(index, begin, end, beginEqualFlag, endEqualFlag);
	for (int i = 0; i < notEqual.size(); i++) {
		try {
			notEqualAddress.push_back(select(index, notEqual[i]));
		}
		catch (NoSuchElementException e) {
			//do nothing
		}
	}
	for (int i = 0; i < notEqualAddress.size(); i++) {
		for (int j = 0; j < result.size(); j++) {
			if (result[j].mBlockOffset == notEqualAddress[i].mBlockOffset && result[j].mFileOffset == notEqualAddress[i].mFileOffset) {
				result.erase(result.begin() + j);
				j--;
			}
		}
	}
	return result;
}

vector<Address> IndexManager::deleteKey(string index, const vector<Key>& valuex, const vector<int>& opx) {
	bool beginEqualFlag = true;
	bool endEqualFlag = true;
	vector<Key> notEqual;
	Key begin;
	Key end;
	TreeHeader treeHeader = getTreeHeader(index);
	Compare com = compares[treeHeader.mKeyType];
	vector<Key> keys;//keys need delete
	vector<Address> result;
	for (int i = 0; i < valuex.size(); i++) {
		switch (opx[i]) {
		case 0:
			result.push_back(select(index, valuex[i]));
			deleteKey(index, valuex[i]);
			return result;
		case 1://op = "<"
			if (end.size() == 0 || com(valuex[i], end) == true) {
				end = valuex[i];
				endEqualFlag = false;
			}
			break;
		case 2://op = ">"
			if (begin.size() == 0 || com(begin, valuex[i]) == true) {
				begin = valuex[i];
				beginEqualFlag = false;
			}
			break;
		case 3://op = "<="
			if (end.size() == 0 || com(valuex[i], end) == true) {
				end = valuex[i];
				endEqualFlag = true;
			}
			break;
		case 4://op = ">="
			if (begin.size() == 0 || com(begin, valuex[i]) == true) {
				begin = valuex[i];
				beginEqualFlag = true;
			}
			break;
		case 5://op = "<>"
			notEqual.push_back(valuex[i]);
			break;
		}
	}
	keys = selectForDelete(index, begin, end);
	if (beginEqualFlag == false && begin.size() != 0) {
		keys.erase(keys.begin());
	}
	if (endEqualFlag == false && end.size() != 0) {
		keys.pop_back();
	}

	for (int i = 0; i < notEqual.size(); i++) {
		for (int j = 0; j < keys.size(); j++) {
			if (keys[j] == notEqual[i]) {
				keys.erase(keys.begin() + j);
				j--;
			}
		}
	}
	for (int i = 0; i < keys.size(); i++) {
		result.push_back(select(index, keys[i]));
		deleteKey(index, keys[i]);
	}

	return result;
}

Address IndexManager::select(string index, const Key& key) throw(NoSuchElementException) {
	string fileName = index + indexPostfix;
	TreeHeader treeHeader = getTreeHeader(index);
	Tree tree(treeHeader.mKeySize, treeHeader.mKeyType, fileName, mBufferManager);
	Address address = tree.searchKey(key);
	address.mFileName = index + tablePostfix;//表的名称规范还需要商议
	return address;
}

vector<Address> IndexManager::select(string index, Key begin, Key end, bool beginEqualFlag, bool endEqualFlag) {
	string fileName = index + indexPostfix;
	TreeHeader treeHeader = getTreeHeader(index);
	Tree tree(treeHeader.mKeySize, treeHeader.mKeyType, fileName, mBufferManager);

	if (begin.size() == 0) {
		begin = tree.findMinKey();
	}
	if (end.size() == 0) {
		end = tree.findMaxKey();
	}

	vector<Address> addresses = tree.searchKey(begin, end, beginEqualFlag, endEqualFlag);
	for (int i = 0; i < addresses.size(); i++) {
		addresses[i].mFileName = index + tablePostfix;
	}
	return addresses;
}

vector<Key> IndexManager::selectForDelete(string index, Key begin, Key end) {
	string fileName = index + indexPostfix;
	TreeHeader treeHeader = getTreeHeader(index);
	Tree tree(treeHeader.mKeySize, treeHeader.mKeyType, fileName, mBufferManager);

	if (begin.size() == 0) {
		begin = tree.findMinKey();
	}
	if (end.size() == 0) {
		end = tree.findMaxKey();
	}

	vector<Key> addresses = tree.searchKeyForDelete(begin, end);
	return addresses;
}

bool IndexManager::insert(string index, const Key& key, Address address) {
	string fileName = index + indexPostfix;
	TreeHeader treeHeader = getTreeHeader(index);
	Tree tree(treeHeader.mKeySize, treeHeader.mKeyType, fileName, mBufferManager);
	Pointer pointer = address.mBlockOffset + (address.mFileOffset << 12);
	try {
		tree.insertKey(key, pointer);
	}
	catch (ElementExistException e) {
		return false;
	}
	return true;
}

bool IndexManager::deleteKey(string index, const Key& key) {
	string fileName = index + indexPostfix;
	TreeHeader treeHeader = getTreeHeader(index);
	Tree tree(treeHeader.mKeySize, treeHeader.mKeyType, fileName, mBufferManager);
	try {
		tree.deleteKey(key);
	}
	catch (NoSuchElementException e) {
		return false;
	}
	return true;
}

TreeHeader IndexManager::getTreeHeader(string index) {
	string fileName = index + indexPostfix;
	Address address;
	address.mFileName = fileName;
	address.mFileOffset = address.mBlockOffset = 0;
	vector<char> data = mBufferManager.read(address, treeHeaderSize);
	char tempData[treeHeaderSize];
	for (int i = 0; i < treeHeaderSize; i++) {
		tempData[i] = data[i];
	}
	TreeHeader treeHeader = *(TreeHeader*)tempData;
	return treeHeader;
}

