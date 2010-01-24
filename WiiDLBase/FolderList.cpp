
#include "FolderList.h"
#include <algorithm>


FolderList::FolderList(void):size(0), head(NULL)
{
}

FolderList::~FolderList(void)
{
}

u64 FolderList::Count()
{
	return size;
}

u64 FolderList::Add(partition_folder * Item)
{
	partition_folder * curFolder = new partition_folder;

	curFolder = Item;
	curFolder->NextFolder = head;
	head = curFolder;
	return size++;
}

partition_folder * FolderList::Retrieve(u64 pos)
{
	partition_folder * curFolder = head;

	for(int i = Count() - 1; i > pos && curFolder != NULL; i--)
	{
		curFolder = curFolder->NextFolder;
	}

	return curFolder;
}

bool FolderList::Delete()
{
	if (head == NULL)
	{
		return false;
	}
	else
	{
		partition_folder * curFolder;

		curFolder = head->NextFolder;
		head->NextFolder = curFolder->NextFolder;
		size--;
		return true;
	}
}

bool FolderList::Delete(u64 pos)
{
	if (Retrieve(pos) == NULL)
		return false;
	else
	{
		Retrieve(pos -1)->NextFolder = Retrieve(pos+1);
		size--;
		return true;
	}
}

u64 FolderList::Find(const char * FolderName)
{
	partition_folder * curFolder;
	u64 i = 0;
	for(curFolder = head; curFolder != NULL; curFolder = curFolder->NextFolder)
	{
		if((strcmp(curFolder->FolderName, FolderName) == 0))
		{
			return i;
		}
		else
		{
			curFolder->NextFolder;
			i++;
		}
	}

	return NULL;
}

//u64 FolderList::AddFileToFolder(const char * fileName, const char * folderName, u64 offset, u64 size)
//{
//	struct partition_folder * f;
//	u64 index = Find(folderName);
//	f = Retrieve(index);
//	if (f == NULL)
//	{
//		// folder doesn't exist. add it
//		f = CreateFolder(folderName);
//		Add(f);
//	}
//	
//	struct partition_file * file;
//	file = (struct partition_file *) (malloc (sizeof (struct partition_file)));
//	memset(file, 0, sizeof (struct partition_file));
//
//	file->FileName = fileName;
//	file->Offset = offset;
//	file->Size = size;
//	return f->Files.Add(file);
//
//}

//partition_folder * FoldersList::CreateFolder(const char * folderName)
//{
//	partition_folder * f;
//	f = (partition_folder *) (malloc (sizeof (partition_folder)));
//	memset(f, 0, sizeof (partition_folder));
//	f->FolderName = folderName;
//
//	return f;
//}





bool partition_folder::AddFile(const char * fileName, u64 offset, u64 size, u32 partNo, int fstRef)
{
	struct PartitionFile * file;
	file = (struct PartitionFile *) (malloc (sizeof (struct PartitionFile)));
	memset(file, 0, sizeof (struct PartitionFile));

	file->FileName = fileName;
	file->Offset = offset;
	file->Size = size;
	file->PartNo = partNo;
	file->FstRef = fstRef;
	return Files.Add(file);
}


