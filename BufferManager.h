#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include <list>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "Block.h"
#include "DBFile.h"
#include "Address.h"
using namespace std;

class BufferManager {
public:
	BufferManager(int capacity = 100);
	virtual ~BufferManager();

	//interfaces
	vector<char> read(Address address, int size);
	vector<char> readBlock(string fileName, int fileOffset);
	void write(Address address, vector<char> data);
	void writeBlock(string fileName, int fileOffset, vector<char> data);
	bool pin(string fileName, int fileOffset);
	bool pin(Address address);//pin the block in which the address is.
	static int getFileBlockNumber(const char* fileName);
	static int getFileBlockNumber(string fileName);
	int appendBlock(const char* fileName, vector<char> data);
	int appendBlock(string fileName, vector<char> data);
	int appendBlock(const Block& block);
	int appendBlock(Block* blockPointer);
	bool createFile(const char* fileName);
	bool createFile(string fileName);
	bool deleteFile(const char* fileName);
	bool deleteFile(string fileName);
	bool clearFile(string fileName);

	//variable access function
	int getCapacity() const;
	void setCapacity(int capacity);
	const list<Block>& getBuffer() const;
	list<Block> getBufferClone();

private:
	list<Block> mBuffer;
	int mCapacity;

	//private function
	void moveBlockToFirst(list<Block>::iterator it);
	bool addBlock(Block block);//add a block into the buffer
	Block readFromFile(string fileName, int fileOffset);
	Block readFromFile(Address address);
	void writeToFile(string fileName, int fileOffset, Block block);
	void writeToFile(Address address, Block block);
	list<Block>::iterator findLastUnpinnedBlock();
	static int getFileSize(const char* fileName);
	static int getFileSize(string fileName);
};

#endif
