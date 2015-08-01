#ifndef _DBFILE_H_
#define _DBFILE_H_

#include <cstdio>
#include <vector>
#include "Block.h"
using namespace std;

class DBFile {
public:
	DBFile();
	DBFile(const char* fileName);
	DBFile(string fileName);
	DBFile(const DBFile& another);
	virtual ~DBFile();

	//interfaces
	bool openFile(const char* fileName);
	bool openFile(string fileName);
	bool createFile(const char* fileName);
	bool createFile(string fileName);
	bool isOpen();
	vector<char> readBlock(int blockId) const;
	bool writeBlock(int blockId, char* blockData = NULL) const;
	bool writeBlock(int blockId, vector<char> blockData) const;
	int addBlock(char* blockData = NULL) const;
	int addBlock(vector<char> blockData) const;
	int getBlockNumber() const;

	//static interfaces
	static bool deleteFile(const char* fileName);
	static bool deleteFile(string fileName);

private:
	FILE* mDBFileHandle;
	string mFileName;

};

#endif
