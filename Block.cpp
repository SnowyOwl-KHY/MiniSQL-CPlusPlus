#include "Block.h"
using namespace std;

Block::Block(string fileName, int fileOffset, vector<char> data, bool isDirty, bool isPinned) : mFileName(fileName), mFileOffset(fileOffset), mData(data), mIsDirty(isDirty), mIsPinned(isPinned) {

}

Block::~Block() {

}

string Block::getFileName() const {
	return mFileName;
}

void Block::setFileName(string fileName) {
	mFileName = fileName;
}

int Block::getFileOffset() const {
	return mFileOffset;
}

void Block::setFileOffset(int fileOffset) {
	mFileOffset = fileOffset;
}

bool Block::isDirty() const {
	return mIsDirty;
}

void Block::markDirty() {
	mIsDirty = true;
}

vector<char>& Block::getData() {
	return mData;
}

vector<char> Block::getDataClone() {
	return mData;
}

const vector<char>& Block::getConstData() const {
	return mData;
}

void Block::setData(vector<char> data) {
	mData = data;
}

bool Block::isPinned() const {
	return mIsPinned;
}

void Block::pin() {
	mIsPinned = true;
}

void Block::unpin() {
	mIsPinned = false;
}
