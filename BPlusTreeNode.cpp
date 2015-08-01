#include "BPlusTreeNode.h"
using namespace std;
using namespace BPlusTree;

Node::Node(int keySize) : mKeySize(keySize) {

	mFileOffset = -1;
}

Node::Node(int keySize, int fileOffset) : mKeySize(keySize), mFileOffset(fileOffset) {

}

Node::Node(int keySize, int fileOffset, const vector<char>& blockData) : mKeySize(keySize), mFileOffset(fileOffset) {

	initialNode(blockData);
}

Node::~Node() {

}

vector<char> Node::toData() {
	vector<char> data;
	char tempNodeHeader[nodeHeaderSize];
	*(NodeHeader*)tempNodeHeader = getNodeHeader();
	for (int i = 0; i < nodeHeaderSize; i++) {
		data.push_back(tempNodeHeader[i]);
	}
	for (int i = 0; i < mKeys.size(); i++) {
		char tempPointer[pointerSize];
		*(Pointer*)tempPointer = mPointers[i];
		for (int j = 0; j < pointerSize; j++) {
			data.push_back(tempPointer[j]);
		}
		for (int j = 0; j < mKeys[i].size(); j++) {
			data.push_back(mKeys[i][j]);
		}
	}
	char tempPointer[pointerSize];
	*(Pointer*)tempPointer = mPointers[mPointers.size() - 1];
	for (int i = 0; i < pointerSize; i++) {
		data.push_back(tempPointer[i]);
	}
	return data;
}

void Node::initialNode(const vector<char>& blockData) {	//使用块内数据来初始化节点
	NodeHeader nodeHeader = getNodeHeader(blockData);
	if (nodeHeader.mIsUsed == false) {
		return;
	}

	mIsLeaf = nodeHeader.mIsLeaf;
	mFather = nodeHeader.mFather;
	for (int i = 0; i < nodeHeader.mKeyNumber; i++) {
		char tempPointer[pointerSize];
		for (int j = 0; j < pointerSize; j++) {
			tempPointer[j] = blockData[nodeHeaderSize + i * (mKeySize + pointerSize) + j];
		}
		mPointers.push_back(*(Pointer*)tempPointer);
		Key tempKey;
		for (int j = 0; j < mKeySize; j++) {
			tempKey.push_back(blockData[nodeHeaderSize + i * (mKeySize + pointerSize) + pointerSize + j]);
		}
		mKeys.push_back(tempKey);
	}
	char tempPointer[pointerSize];
	for (int j = 0; j < pointerSize; j++) {
		tempPointer[j] = blockData[nodeHeaderSize + nodeHeader.mKeyNumber * (mKeySize + pointerSize) + j];
	}
	mPointers.push_back(*(Pointer*)tempPointer);
}

NodeHeader Node::getNodeHeader() {
	NodeHeader nodeHeader;
	nodeHeader.mIsUsed = true;
	nodeHeader.mIsLeaf = mIsLeaf;
	nodeHeader.mKeyNumber = mKeys.size();
	nodeHeader.mFather = mFather;
	return nodeHeader;
}

NodeHeader Node::getNodeHeader(const vector<char>& blockData) {
	char temp[nodeHeaderSize];
	for (int i = 0; i < nodeHeaderSize; i++) {
		temp[i] = blockData[i];
	}
	return *(NodeHeader*)temp;
}


