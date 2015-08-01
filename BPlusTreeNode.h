#ifndef _BPLUSTREENODE_H
#define _BPLUSTREENODE_H
#include <algorithm>
#include <cassert>
#include "BufferManager.h"
using namespace std;

#define NULLPOINTER -1


namespace BPlusTree {

	typedef vector<char> Key;	//索引值类型
	typedef int Pointer;		//指针类型

	struct NodeHeader {			//块头
		bool mIsUsed;
		bool mIsLeaf;
		int mKeyNumber;
		Pointer mFather;
	};

	const int nodeHeaderSize = sizeof(NodeHeader);
	const int pointerSize = sizeof(Pointer);

	class Node {
	public:
		Node(int keySize);
		Node(int keySize, int fileOffset);
		Node(int keySize, int fileOffset, const vector<char>& blockData);
		virtual ~Node();
		friend class Tree;

		//member function
		vector<char> toData();

	private:
		int mKeySize;
		bool mIsLeaf;
		int mFileOffset;
		Pointer mFather;
		vector<Key> mKeys;
		vector<Pointer> mPointers;

		//member function
		void initialNode(const vector<char>& blockData);
		NodeHeader getNodeHeader();
		static NodeHeader getNodeHeader(const vector<char>& blockData);
	};
}

#endif

