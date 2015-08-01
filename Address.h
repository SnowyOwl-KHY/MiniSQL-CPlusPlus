#pragma once
#ifndef ADDRESS_H
#define ADDRESS_H
#include<string>
using namespace std;

class Address
{
//private:
	
public:
	string mFileName;
	int mFileOffset;
	int mBlockOffset;
	Address(){
		mFileName = "";
		mFileOffset = 0;
		mBlockOffset = 0;
	}
	Address(string f,int b,int o){
		mFileName = f;
		mFileOffset = b;
		mBlockOffset = o;
	}
	~Address();
	void setAddr(string f, int b, int o){
		mFileName = f;
		mFileOffset = b;
		mBlockOffset = o;
	}
	int isNullAddr(){
		if (mFileName.size() == 0)
			return 1;
		else
			return 0;
	}
	int isHead(){
		if (mFileOffset==0&&mBlockOffset==0)
			return 1;
		else
			return 0;
	}
	void setAddrFile(string f){
		mFileName = f;
	}
};
#endif//ADDRESS_H

