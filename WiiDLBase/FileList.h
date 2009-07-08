#pragma once
#include "Global.h"

struct partition_file 
{
	u64 Offset;
	u64 Size;
	const char * FileName;
	const char * DirectoryName;

	partition_file * NextFile;
};


class FileList
{
public:
	FileList(void);
	~FileList(void);
	u64 Count();
	u64 Add(partition_file * Item);
	partition_file * Retrieve(u64 pos);
	bool Delete();
	bool Delete(u64 pos);
	u64 Find(const char * FileName, const char * DirectoryName);


private:
	u64 size;
	partition_file * head;

};


