#include "Global.h"
#include "aes.h"
#include <sys/stat.h>

enum tmd_sig {
        SIG_UNKNOWN = 0,
        SIG_RSA_2048,
        SIG_RSA_4096
};

struct tmd_content {
        u32 cid;
        u16 index;
        u16 type;
        u64 size;
        u8 hash[20];
};

struct tmd {
        enum tmd_sig sig_type; 
        u8 *sig;
        char issuer[64];
        u8 version;
        u8 ca_crl_version;
        u8 signer_crl_version;
        u64 sys_version;
        u64 title_id;
        u32 title_type;
        u16 group_id;
        u32 access_rights;
        u16 title_version;
        u16 num_contents;
        u16 boot_index;
        struct tmd_content *contents;
};

struct part_header {
        char DiscId; // 0x0: R = Revolution (Wii), S = Wii, G = Gamecube, U = Utility Disc / Gamecube,
					 //		 D = Gamecube Demo Disc, P = Gamecube Promo Disc, 0 = Diagnostic Disc (autoboot), 
					 //		 1 = Diagnostic Disc, 4 = Backup Disc, _ = Wii Fit Channel Installer

        u8 IsGC;  // true if magic word at 0x1c = 0xc2339f3d
        u8 IsWii; // true if magic word at 0x18 = 0x5D1C9EA3
		bool KoreanKey; // true if the partition ticket common key index at 0x1f1 = 1
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
        u64 offset;

        struct part_header header;

        u64 appldr_size;

        u8 is_encrypted;

        u64 tmd_offset;
        u64 tmd_size;

        struct tmd * tmd;

		u64	h3_offset;

        char title_id_str[17];

        enum partition_type type;
        char chan_id[5];

        char key_c[35];
        AES_KEY key;

		u8 title_key[16];

        u64 data_offset;
        u64 data_size;

        u64 cert_offset;
        u64 cert_size;

        u8 dec_buffer[0x8000];

        u32 cached_block;
        u8 cache[0x7c00];
};

struct image_file {
 
		FILE * File;
		long long DiscOffset;
		long long ImageSize;
        u8 is_wii;

		unsigned char * pFreeTable;
		bool readOnly;

        u32 nparts;
        struct partition *parts;

		//TODO: Enable this for cross platform
        //struct _stat st; 

        u64 nfiles;
        u64 nbytes;

		u8	PrimaryCount;
		u8	SecondaryCount;
		u8	TertiaryCount;
		u8	QuaternaryCount;

		u64 Primary_tbl_offset;
		u64	Secondary_tbl_offset;
		u64	Tertiary_tbl_offset;
		u64	Quaternary_tbl_offset;

        AES_KEY key;
};