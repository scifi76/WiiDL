#pragma once
// Disc class - represents a Wii disc image (ISO) and contains most of the methods for
// working with the image

#include <fstream>
#include <string>
#include "Structs.h"
#include "aes.h"
#include "FileList.h"



#define DISC_EXPORTS 

// This ifdef block allows the header to export the DLL's methods when compiled
// from an app that defines DISC_EXPORTS (eg the Base). If DISC_EXPORTS is not
// defined then the methods will be imported instead. Anything defined with 
// DISC_API will be made available for export/import. Clever stuff.
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




using namespace std;

///<summary>
/// Represents a Wii disc image (ISO) and contains most of the methods for working with the image
///</summary>
class DISC_API Disc
{
	public:
		// Constructor / Destructor
		Disc(char * IsoFileName);
		~Disc(void);

		// public variables
		bool IsLoaded;
		struct image_file * Image; // stores image file info. It will be populated when calling Open()

		// public methods
		bool Load(bool readOnly);
		bool CloseFile();
		string GetLastError();
		u64 Read (unsigned char * buffer, size_t size, u64 offset, bool markUsed = true);
		u64 ReadFromPartition(unsigned char * buffer, size_t size, u32 partNo, u64 offset);
		struct part_header * ParseImageHeader(u8 * inputData);
		void LoadKey(bool korean);
		int MarkAsUsed(u64 nOffset, u64 nSize);
		int MarkAsUsedCrypto(u64 partOffset, u64 offset, u64 size, bool isEncrypted);
		void MarkAsUnused(u64 nOffset, u64 nSize);
		int ParseImage();
		int ParsePartitions();
		tmd * TmdLoad(u32 partNo);
		bool ExtractFile(const char * destFilename, PartitionFile * file, bool decrypt = true);
		bool ReplaceFile(const char * inputFilename, PartitionFile * file, bool encrypt);
		string ImageFileName; // the name of the isofile
		
	private:
		// private variables
		
		const char * _lastErr; // used to store the last error message that occured
		
		unsigned char * _blankSector; // contains data needed to write an empty cluster with 0xFF
		unsigned char * _blankSector0; // as above but with 0x0
		
		//private methods
		int DecryptPartitionBlock(u32 partNo, u32 block);
		//void AddFileToPart(const char * fileName, const char * directoryName, partition * part, u64 offset, u64 size);
		void AddFolderToPart(const char * folderName);
		u32 ParseFst(u8 * fst, const char * names, partition_folder * currentFolder, u32 i, struct tree * tree, u32 partNo);
		bool WriteData(int partition, u64 offset, u64 size, u8 *in, FILE * fIn);
		bool WriteClusters(int partNo, u32 cluster, u8 * in, u32 clusterOffset, u32 bytesToWrite, FILE * fIn);
		int GetPartitionClusterCount(int partNo);
		int ReadCluster(int partNo, int clusterNo, u8 * data, u8 * header);
		void aes_cbc_enc(u8 *in, u8 *out, u32 len, u8 *key, u8 *iv);
		void aes_cbc_dec(u8 *in, u8 *out, u32 len, u8 *key, u8 *iv);
		void sha1(u8 *data, u32 len, u8 *hash);
		bool DiscWriteDirect(u64 nOffset, u8 *pData, unsigned int nSize);
		bool TruchaSignPartition(int partNo);
		u64 FindRequiredFreeSpaceInPartition(int partNo, u32 nRequiredSize);
		bool CheckForFreeSpace(u32 partNo, u64 nOffset, u32 nBlocks);
		void Write32( u8 *p, u32 nVal);

};

