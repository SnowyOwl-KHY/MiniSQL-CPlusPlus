#include "BufferManager.h"
using namespace std;

BufferManager::BufferManager(int capacity) : mCapacity(capacity) {

}

BufferManager::~BufferManager() {
	for (auto iterator = mBuffer.begin(); iterator != mBuffer.end(); iterator++) {
		if (iterator->isDirty()) {
			writeToFile(iterator->getFileName(), iterator->getFileOffset(), *iterator);
		}
	}
}

vector<char> BufferManager::read(Address address, int size) {
	string fileName = address.mFileName;
	int fileOffset = address.mFileOffset;
	int blockOffset = address.mBlockOffset;
	for (auto it = mBuffer.begin(); it != mBuffer.end(); it++) {
		if (fileName == it->getFileName() && fileOffset == it->getFileOffset()) {
			vector<char> data;
			for (int i = 0; i < size; i++) {
				data.push_back(it->getData()[blockOffset + i]);
			}
			moveBlockToFirst(it);
			return data;
		}
	}
	//no block match
	//read data from file
	Block newBlock = readFromFile(fileName, fileOffset);
	vector<char> data;
	for (int i = 0; i < size; i++) {
		data.push_back(newBlock.getData()[blockOffset + i]);
	}
	addBlock(newBlock);
	return data;
}

vector<char> BufferManager::readBlock(string fileName, int fileOffset) {
	Address address;
	address.mFileName = fileName;
	address.mFileOffset = fileOffset;
	address.mBlockOffset = 0;
	return read(address, blockSize);
}

void BufferManager::write(Address address, vector<char> data) {
	if (address.mBlockOffset + data.size() > blockSize) {
		throw 1;
	}
	string fileName = address.mFileName;
	int fileOffset = address.mFileOffset;
	int blockOffset = address.mBlockOffset;
	int blockNumber = getFileBlockNumber(fileName);
	if (blockNumber <= fileOffset) {
		vector<char> tempBlockData(blockSize, '\0');
		for (int i = blockNumber; i <= fileOffset; i++) {
			appendBlock(fileName, tempBlockData);
		}
	}
	for (auto it = mBuffer.begin(); it != mBuffer.end(); it++) {
		if (fileName == it->getFileName() && fileOffset == it->getFileOffset()) {
			for (int i = 0; i < data.size(); i++) {
				it->getData()[blockOffset + i] = data[i];
			}
			it->markDirty();
			moveBlockToFirst(it);
			return;
		}
	}
	//no block match
	//read data from file and then write to it
	Block newBlock = readFromFile(fileName, fileOffset);
	for (int i = 0; i < data.size(); i++) {
		newBlock.getData()[blockOffset + i] = data[i];
	}
	newBlock.markDirty();
	addBlock(newBlock);
}

void BufferManager::writeBlock(string fileName, int fileOffset, vector<char> data) {

	if (data.size() < blockSize) {
		for (int i = data.size(); i < blockSize; i++) {
			data.push_back('\0');
		}
	}
	Address address;
	address.mFileName = fileName;
	address.mFileOffset = fileOffset;
	address.mBlockOffset = 0;
	write(address, data);
}

bool BufferManager::pin(string fileName, int fileOffset) {
	for (auto it = mBuffer.begin(); it != mBuffer.end(); it++) {
		if (it->getFileName() == fileName && it->getFileOffset() == fileOffset) {
			it->pin();
			moveBlockToFirst(it);
			return true;
		}
	}
	//no block match
	//read block from file and then pin it
	Block newBlock = readFromFile(fileName, fileOffset);
	newBlock.pin();
	return addBlock(newBlock);
}

bool BufferManager::pin(Address address) {
	return pin(address.mFileName, address.mFileOffset);
}

int BufferManager::getFileBlockNumber(const char* fileName) {
	return getFileSize(fileName) / blockSize;
}

int BufferManager::getFileBlockNumber(string fileName) {
	return getFileBlockNumber(fileName.c_str());
}

int BufferManager::appendBlock(const char* fileName, vector<char> data) {
	DBFile file(fileName);
	for (int i = data.size(); i < blockSize; i++) {
		data.push_back('\0');
	}
	int fileOffset = file.addBlock(data);
	Block block(fileName, fileOffset, data);
	addBlock(block);
	return fileOffset;
}

int BufferManager::appendBlock(string fileName, vector<char> data) {
	return appendBlock(fileName.c_str(), data);
}

int BufferManager::appendBlock(const Block& block) {
	return appendBlock(block.getFileName(), block.getConstData());
}

int BufferManager::appendBlock(Block* blockPointer) {
	blockPointer->setFileOffset(appendBlock(blockPointer->getFileName(), blockPointer->getData()));
	return blockPointer->getFileOffset();
}

bool BufferManager::createFile(const char* fileName) {
	DBFile file;
	return file.createFile(fileName);
}

bool BufferManager::createFile(string fileName) {
	return createFile(fileName.c_str());
}

bool BufferManager::deleteFile(const char* fileName) {
	return deleteFile(string(fileName));
}

bool BufferManager::deleteFile(string fileName) {
	for (auto it = mBuffer.begin(); it != mBuffer.end();) {
		if (it->getFileName() == fileName) {
			auto tempIt = it;
			it++;
			mBuffer.erase(tempIt);
		}
		else {
			it++;
		}
	}
	return DBFile::deleteFile(fileName);
}

bool BufferManager::clearFile(string fileName) {
	for (auto it = mBuffer.begin(); it != mBuffer.end(); ) {
		if (it->getFileName() == fileName) {
			auto tempIt = it;
			it++;
			mBuffer.erase(tempIt);
		}
		else {
			it++; 
		}
	}
	DBFile::deleteFile(fileName);
	DBFile file;
	file.createFile(fileName);
	return true;
}

int BufferManager::getCapacity() const {
	return mCapacity;
}

void BufferManager::setCapacity(int capacity) {
	mCapacity = capacity;
}

const list<Block>& BufferManager::getBuffer() const {
	return mBuffer;
}

list<Block> BufferManager::getBufferClone() {
	return mBuffer;
}

void BufferManager::moveBlockToFirst(list<Block>::iterator it) {
	mBuffer.push_front(*it);
	mBuffer.erase(it);
}

bool BufferManager::addBlock(Block block) {
	mBuffer.push_front(block);
	if (mBuffer.size() > mCapacity) {
		list<Block>::iterator iterator = findLastUnpinnedBlock();
		if (iterator == mBuffer.end()) {
			return false;
		}
		if (iterator->isDirty()) {
			writeToFile(iterator->getFileName(), iterator->getFileOffset(), *iterator);
		}
		mBuffer.erase(iterator);
	}
	return true;
}

Block BufferManager::readFromFile(string fileName, int fileOffset) {
	DBFile file(fileName);
	vector<char> data = file.readBlock(fileOffset);
	return Block(fileName, fileOffset, data);
}

Block BufferManager::readFromFile(Address address) {
	return readFromFile(address.mFileName, address.mFileOffset);
}

void BufferManager::writeToFile(string fileName, int fileOffset, Block block) {
	DBFile file(fileName);
	file.writeBlock(fileOffset, block.getData());
}

void BufferManager::writeToFile(Address address, Block block) {
	writeToFile(address.mFileName, address.mFileOffset, block);
}

list<Block>::iterator BufferManager::findLastUnpinnedBlock() {
	list<Block>::iterator iterator = mBuffer.end();
	for (auto it = mBuffer.begin(); it != mBuffer.end(); it++) {
		//find the last block that is not pinned
		if (!it->isPinned()) {
			iterator = it;
		}
	}
	return iterator;
}

int BufferManager::getFileSize(const char* fileName) {
	struct stat fStat;
	if (stat(fileName, &fStat) == -1) {
		return -1;
	}
	return (int)fStat.st_size;
}

int BufferManager::getFileSize(string fileName) {
	return getFileSize(fileName.c_str());
}
