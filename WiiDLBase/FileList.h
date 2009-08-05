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

class DISC_API FileList
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




