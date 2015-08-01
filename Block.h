#ifndef _BLOCK_H
#define _BLOCK_H
#include <string>
#include <vector>
using namespace std;
const int blockSize = 4096;

class Block {
public:
	Block(string fileName, int fileOffset, vector<char> data, bool isDirty = false, bool isPinned = false);
	virtual ~Block();

	//interfaces

	//variable access function
	string getFileName() const;
	void setFileName(string fileName);
	int getFileOffset() const;
	void setFileOffset(int fileOffset);
	bool isDirty() const;
	void markDirty();
	vector<char>& getData();
	vector<char> getDataClone();
	const vector<char>& getConstData() const;
	void setData(vector<char> data);
	bool isPinned() const;
	void pin();
	void unpin();

private:
	string mFileName;
	int mFileOffset;
	bool mIsDirty;
	vector<char> mData;
	bool mIsPinned;
};

#endif
