#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H
#include "BPlusTreeNode.h"


namespace BPlusTree {

	bool compareInt(const Key& key1, const Key& key2);
	bool compareChar(const Key& key1, const Key& key2);
	bool compareFloat(const Key& key1, const Key& key2);

	typedef bool(*Compare)(const Key&, const Key&);//比较函数
	
	enum KeyType {//索引值类型
		INT,
		CHAR,
		FLOAT,
		UNKNOWN
	};

	struct TreeHeader {//索引文件头
		Pointer mRoot;
		KeyType mKeyType;
		int mKeySize;
	};

	const int treeHeaderSize = sizeof(TreeHeader);

	class Tree {
	public:
		Tree(int keySize, KeyType keyType, string fileName, BufferManager& bufferManager);
		virtual ~Tree();

		//interfaces
		Address searchKey(const Key& key);
		vector<Address> searchKey(const Key& begin, const Key& end, bool beginEqualFlag, bool endEqualFlag);
		void insertKey(const Key& key, Pointer recordPointer);
		void deleteKey(const Key& key);
		void updateKey(const Key& oldKey, const Key& newKey, Pointer recordPointer);
		Key findMinKey();
		Key findMaxKey();

		Key searchKeyForDelete(const Key& key);
		vector<Key> searchKeyForDelete(const Key& begin, const Key& end);

		//variable access function
		int getKeySize();
		KeyType getKeyType();
		int getOrder();
		string getFileName();
		void setFileName(string fileName);
		Compare getCompare();
		void setCompare(Compare compare);

	private:
		Pointer mRoot;
		const int mKeySize;
		const KeyType mKeyType;
		const int mOrder;
		string mFileName;
		Compare mCompare;//for the comparing of keys
		BufferManager& mBufferManager;

		void loadRoot();
		TreeHeader getTreeHeader();
		void deleteNode(Pointer pointer);
		Node getNode(Pointer pointer);

		Node findMinLeaf();
		Node findMaxLeaf();
		Pointer search(const Key& k);
		vector<Pointer> search(const Key& begin, const Key& end, bool beginEqualFlag, bool endEqualFlag);
		Node findShouldContain(const Key& k);
		Node find(const Key& k);
		Node find(const Key& k, Pointer p);
		void insert(const Key& k, Pointer p);
		void insertInParent(Node& node1, const Key& k, Node& node2);
		void deleteKey(const Key& k, Pointer p);
		void deleteEntry(Node& n, const Key& k, Pointer p);

	};

	class NoSuchElementException {
	public:
		void printException() {
			printf("No such element in the tree.\n");
		}
	};

	class ElementExistException {
	public:
		void printException() {
			printf("The element has existed.\n");
		}
	};

	class TreeEmptyException {
	public:
		void printException() {
			printf("The tree is empty.\n");
		}
	};

}


#endif
