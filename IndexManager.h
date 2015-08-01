#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include "BPlusTree.h"
using namespace std;
using namespace BPlusTree;

class IndexManager {

public:
	IndexManager(BufferManager& mBufferManager);
	virtual ~IndexManager();

	bool createIndex(string index, KeyType keyType, int keySize);
	bool dropIndex(string index);
	bool rebuildIndex(string index);
	vector<Address> select(string index, const vector<Key>& valuex, const vector<int>& opx);
	bool insert(string index, const Key& key, Address address);
	vector<Address> deleteKey(string index, const vector<Key>& valuex, const vector<int>& opx);


	bool deleteKey(string index, const Key& key);
	Address select(string index, const Key& key);
	vector<Address> select(string index, Key begin, Key end, bool beginEqualFlag, bool endEqualFlag);

	vector<Key> selectForDelete(string index, Key begin, Key end);

private:
	BufferManager& mBufferManager;

	TreeHeader IndexManager::getTreeHeader(string index);

};

#endif
