#pragma once
#include "Global.h"


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

struct PartitionFile
{
public:
	
	u64 Offset;
	u64 Size;
	u32 PartNo;
	int FstRef;
	const char * FileName;

	PartitionFile * NextFile;


};


class DISC_API FileList
{
public:
	FileList(void);
	~FileList(void);
	u64 Count();
	u64 Add(PartitionFile * Item);
	PartitionFile * Retrieve(u64 pos);
	bool Delete();
	bool Delete(u64 pos);
	u64 Find(const char * FileName);
	


private:
	u64 size;
	PartitionFile * head;

};




