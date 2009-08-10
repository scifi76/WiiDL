
#include "FileList.h"



FileList::FileList(void):size(0), head(NULL)
{
}

FileList::~FileList(void)
{
}


u64 FileList::Count()
{
	return size;
}

u64 FileList::Add(partition_file * Item)
{
	partition_file * curFile = new partition_file;

	curFile = Item;
	curFile->NextFile = head;
	head = curFile;
	return size++;

}

partition_file * FileList::Retrieve(u64 pos)
{
	partition_file * curFile = head;
	
	for(int i = Count() - 1; i > pos && curFile != NULL; i--)
	{
		curFile = curFile->NextFile;
	}

	return curFile;

}

bool FileList::Delete()
{

	if( head == NULL )
	{
		return false;
	}
	else
	{
		partition_file * curFile;

		curFile = head->NextFile;
		head->NextFile = curFile->NextFile;
		size--;
		return true;
	}

}

bool FileList::Delete(u64 pos)
{
	if( Retrieve(pos) == NULL )
		return false;
	else
	{
		Retrieve(pos - 1)->NextFile = Retrieve(pos+1);
		size--;
		return true;
	}

}

u64 FileList::Find(const char * FileName)
{
	partition_file * curFile;
	u64 i = 0;
	for(curFile = head; curFile != NULL; curFile = curFile->NextFile)
	{
		if((strcmp(curFile->FileName, FileName) == 0))
		{
			return i;
		}
		else
		{
			curFile->NextFile;
			i++;
		}
	}

	return NULL;

}

