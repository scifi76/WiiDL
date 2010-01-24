#include "Global.h"
#include "aes.h"
#include <sys/stat.h>
#include "FolderList.h"


enum tmd_sig {
        SIG_UNKNOWN = 0,
        SIG_RSA_2048,
        SIG_RSA_4096
};

struct tmd_content {
        u32 Cid;
        u16 Index;
        u16 Type;
        u64 Size;
        u8 Hash[20];
};

struct tmd {
        enum tmd_sig SigType; 
        u8 * Sig;
        char Issuer[64];
        u8 Version;
        u8 CaCrlVersion;
        u8 SignerCrlVersion;
        u64 SysVersion;
        u64 TitleId;
        u32 TitleType;
        u16 GroupId;
        u32 AccessRights;
        u16 TitleVersion;
        u16 NumContents;
        u16 BootIndex;
        struct tmd_content * Contents;
};

struct part_header {
        char DiscId; // 0x0: R = Revolution (Wii), S = Wii, G = Gamecube, U = Utility Disc / Gamecube,
					 //		 D = Gamecube Demo Disc, P = Gamecube Promo Disc, 0 = Diagnostic Disc (autoboot), 
					 //		 1 = Diagnostic Disc, 4 = Backup Disc, _ = Wii Fit Channel Installer

        u8 IsGC;  // true if magic word at 0x1c = 0xc2339f3d
        u8 IsWii; // true if magic word at 0x18 = 0x5D1C9EA3
		bool IsKoreanKey; // true if the partition ticket common key index at 0x1f1 = 1
        char Gamecode[2]; // the 2 character game code from  0x1
        char Region; // the disc region code from  0x3
        char Publisher[2]; // the 2 character publisher code from 0x4

        u8 HasMagic; // true if offset 0x1c = 0xc2339f3d (gamecube) OR offset 0x18 = 0x5D1C9EA3 (wii)
        char GameTitle[0x60]; // Game title from 0x20

        u64 DolOffset; // the DOL offset
        u64 DolSize;  // DOL size

        u64 FstOffset; // fst.bin offset
        u64 FstSize;	// fst.bin size
};

enum partition_type {
        PART_UNKNOWN = 0,
        PART_DATA,
        PART_UPDATE,
        PART_INSTALLER,
        PART_VC
};

struct partition {
        u64 Offset;

        struct part_header * Header;
		FolderList Folders;

        u64 AppldrSize;

        bool IsEncrypted;
		bool IsValid;

        u64 TmdOffset;
        u64 TmdSize;

        struct tmd * Tmd;

		u64	H3Offset;

        char TitleIdString[17];

        enum partition_type Type;
        char ChannelId[5];

        char KeyC[35];
        AES_KEY Key;

		u8 TitleKey[16];

        u64 DataOffset;
        u64 DataSize;

        u64 CertOffset;
        u64 CertSize;

        u8 DecBuffer[0x8000];

        u32 CachedBlock;
        u8 Cache[0x7c00];
};


class image_file {
public:
 		FILE * File;
		u64 DiscOffset;
		u64 ImageSize;

		unsigned char * FreeClusterTable;
		bool IsReadOnly;

		struct part_header * ImageHeader;

        u32 PartitionCount;
        struct partition * Partitions;

		//TODO: Enable this for cross platform
        //struct _stat st; 

        u64 FileCount;
        u64 BytesCount;

		u8	PrimaryCount;
		u8	SecondaryCount;
		u8	TertiaryCount;
		u8	QuaternaryCount;

		u64 PrimaryTblOffset;
		u64	SecondaryTblOffset;
		u64	TertiaryTblOffset;
		u64	QuaternaryTblOffset;

        AES_KEY CommonKey;
};