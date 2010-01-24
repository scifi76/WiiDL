#pragma once
#include "FileList.h"


#define DISC_EXPORTS 

#if defined (__GNUC__) && defined(__unix__)
	#define DISC_API __attribute__ ((__visibility__("default")))
#elif defined (_MSC_VER)
	#ifdef DISC_EXPORTS
		#define DISC_API __declspec(dllexport)
		
	#else
		#define DISC_API __declspec(dllimport)
		#pragma message("Linking to WiiBaseDL.lib...")
		#pragma comment(lib, "WiiDLBase.lib") 
	#endif
#endif

class partition_folder;




class DISC_API FolderList
{
public:
	FolderList(void);
	~FolderList(void);
	u64 Count();
	u64 Add(partition_folder * Item);
	partition_folder * Retrieve(u64 pos);
	bool Delete();
	bool Delete(u64 pos);
	u64 Find(const char * FolderName);
	//u64 FolderList::AddFileToFolder(const char * fileName, const char * folderName, u64 offset, u64 size);
	

private:
	u64 size;
	partition_folder * head;
};

class partition_folder
{
public:
	const char * FolderName;
	FileList Files;
	FolderList SubFolders;
	bool AddFile(const char * fileName, u64 offset, u64 size, u32 partNo, int fstRef);

	partition_folder * NextFolder;
};
