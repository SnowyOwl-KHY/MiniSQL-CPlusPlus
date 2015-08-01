#include "DBFile.h"

DBFile::DBFile() {
	mDBFileHandle = NULL;
}

DBFile::DBFile(const char* fileName) {
	openFile(fileName);
}

DBFile::DBFile(string fileName) {
	openFile(fileName);
}

DBFile::DBFile(const DBFile& another) {
	openFile(another.mFileName);
	mFileName = another.mFileName;
}

DBFile::~DBFile() {
	if (mDBFileHandle != NULL) {
		fclose(mDBFileHandle);
	}
}

bool DBFile::openFile(const char* fileName) {
	fopen_s(&mDBFileHandle, fileName, "rb+");
	mFileName = fileName;
	return mDBFileHandle != NULL;
}

bool DBFile::openFile(string fileName) {
	return openFile(fileName.c_str());
}

bool DBFile::createFile(const char* fileName) {
	fopen_s(&mDBFileHandle, fileName, "a");
	return mDBFileHandle != NULL;
}

bool DBFile::createFile(string fileName) {
	return createFile(fileName.c_str());
}

bool DBFile::isOpen() {
	return mDBFileHandle >= 0;
}

vector<char> DBFile::readBlock(int blockId) const {
	//use lseek to locate the position
	char tempBuffer[blockSize];
	fseek(mDBFileHandle, blockId * blockSize, SEEK_SET);
	fread(tempBuffer, 1, blockSize, mDBFileHandle);
	vector<char> blockData;
	for (int i = 0; i < blockSize; i++) {
		blockData.push_back(tempBuffer[i]);
	}
	return blockData;
}

bool DBFile::writeBlock(int blockId, char* blockData) const {
	if (blockData == NULL) {
		return false;
	}

	fseek(mDBFileHandle, blockId * blockSize, SEEK_SET);
	return fwrite(blockData, 1, blockSize, mDBFileHandle) == blockSize;
}

bool DBFile::writeBlock(int blockId, vector<char> blockData) const {
	char tempData[blockSize];
	for (int i = 0; i < blockSize; i++) {
		tempData[i] = blockData[i];
	}
	return writeBlock(blockId, tempData);
}

int DBFile::addBlock(char* blockData) const {
	if (blockData == NULL) {
		return -1;
	}

	fseek(mDBFileHandle, 0L, SEEK_END);
	//now blockData != NULL, write data into the block
	if (fwrite(blockData, 1, blockSize, mDBFileHandle) != blockSize) {
		return -1;
	}
	else {
		//return blockId
		return ftell(mDBFileHandle) / blockSize - 1;
	}
}

int DBFile::addBlock(vector<char> blockData) const {
	if (blockData.size() < blockSize) {
		for (int i = blockData.size(); i < blockSize; i++) {
			blockData.push_back('\0');//不满一个块用0填充
		}
	}
	char tempData[blockSize];
	for (int i = 0; i < blockSize; i++) {//超过一个块的部分忽略
		tempData[i] = blockData[i];
	}
	return addBlock(tempData);
}

int DBFile::getBlockNumber() const {
	fseek(mDBFileHandle, 0, SEEK_END);
	return ftell(mDBFileHandle) / blockSize;
}

bool DBFile::deleteFile(const char* fileName) {
	return remove(fileName) == 0;
}

bool DBFile::deleteFile(string fileName) {
	return deleteFile(fileName.c_str());
}
