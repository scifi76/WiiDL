#pragma once
#include "Global.h"
#include "FileList.h"
#include <string>

struct partition_folder
{
	const char * FolderName;
	FileList Files;

	partition_folder * NextFolder;
};

#define DISC_EXPORTS 

#if defined (__GNUC__) && defined(__unix__)
	#define DISC_API __attribute__ ((__visibility__("default")))
#elif defined (_MSC_VER)
	#include <io.h>
	#define fseeko64(fm, pos, type) _lseeki64(_fileno(fm), (pos), (type))
	
	#ifdef DISC_EXPORTS
		#define DISC_API __declspec(dllexport)
		
	#else
		#define DISC_API __declspec(dllimport)
		#pragma message("Linking to WiiBaseDL.lib...")
		#pragma comment(lib, "WiiDLBase.lib") 
	#endif
#endif

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
	u64 FolderList::AddFileToFolder(const char * fileName, const char * folderName, u64 offset, u64 size);

private:
	u64 size;
	partition_folder * head;
};
