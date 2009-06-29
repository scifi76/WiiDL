
#include "Disc.h"
#include "Utils.h"
#include <string.h>
#include "Global.h"





///<summary>
/// Constructor. Creates a Disc object
///<param name="IsoFilename">The path of the Wii ISO file that the Disc object will access</param>
///</summary>
Disc::Disc(char * IsoFileName)
{
	IsLoaded = false;
	_isoFileName = IsoFileName;

	_blankSector = (unsigned char *) malloc(0x8000);
	memset(_blankSector, 0xFF, 0x8000);

	_blankSector0 = (unsigned char *) malloc(0x8000);
	memset (_blankSector0, 0, 0x8000);
}

///<summary>
/// Destructor. Disposes of the Disc object
///</summary>
Disc::~Disc()
{
	// Close the ISO file if it's open
	CloseFile();

	
	free(_image->FreeClusterTable);
	free(&_image->ImageHeader);
	free(_image->File);
	free(_image);
	free(_blankSector);
	free(_blankSector0);
}

///<summary>
/// Gets the last error message that occured
///<returns>The most recent error message</returns>
///</summary>
const char * Disc::GetLastError()
{
	// return the latest error message
	return _lastErr;
}

///<summary>
/// Opens the Wii ISO
///<param name="readOnly">Whether or not the ISO should be opened in read only mode. Opening in read only mode will prevent the image file from being locked to other applications but any write actions to the image will fail</param>
///<returns>True if the ISO was successfully opened. Otherwise false</returns>
///</summary>
bool Disc::Load(bool readOnly)
{
	if (!IsLoaded) // dont bother if it's already open
	{
		try
		{
			
			string isoExtension;
			FILE * fIsoFile; // a stream object for accessing the file
			u64 discOffset; // the offset within the ISO file where the disc's data starts
			u64 imageSize; // the size of the disc image

			string mode;
			if (readOnly)
			{
				mode = "rb";
			}
			else
			{
				mode = "r+b";
				
			}
			// try to open the iso
			fIsoFile = fopen(_isoFileName.c_str(), mode.c_str());

			//check the file opened ok
			if (fIsoFile != NULL)
			{
				// check if this is a devkitimage (.RVM)
				isoExtension = Utils::StringToUpper(Utils::GetFileExtension(_isoFileName));
				if (isoExtension == "RVM")
				{
					// devkit images have an additional 32k header. We need to skip it
					discOffset = 0x8000;
				}
				else
				{
					// no devkit image, so no offset
					discOffset = 0;
				}


				// get the size of the file not including the devkit header
				imageSize = fseeko64(fIsoFile, 0L, SEEK_END);
				#if defined (__GNUC__) && defined(__unix__)
					// fseeko64 in unix always returns 0 on success so we need to use ftell to get the file position
					imageSize = ftello64(fIsoFile);
				#endif
				imageSize -= discOffset;
				
				// get the image info and store it in _image
				struct part_header * header;
		        u8 buffer[0x440];
				// allocate the memory
				_image = (struct image_file *) malloc (sizeof (struct image_file));
		
				if (!_image) // memory allocation failed
					throw std::ios::failure("Unable to allocate memory for image file struct");

				memset (_image, 0, sizeof (struct image_file));
				_image->File = fIsoFile; // add the file pointer to the image structure

				// close the file (for now)
				Disc::CloseFile();
				
				// add the read only flag to the struct
				_image->IsReadOnly = readOnly;

				// set the image size
				_image->ImageSize = imageSize;

				// allocate the FreeClusterTable
				_image->FreeClusterTable = (unsigned char *) malloc(imageSize);
				//set all clusters to free for now
				memset(_image->FreeClusterTable, 0, (_image->ImageSize / (u64)(0x8000)) *2L);
				// then set the header size to used
				MarkAsUsed(0, 0x50000);

				// read the header data
				if (Disc::Read(buffer, 0x440, 0, false) == -1) 
					throw std::ios::failure("Unable to read header from image file");

				// header data read ok if we got here so allocate some memory to store it in
				header = (struct part_header *) (malloc (sizeof (struct part_header)));

				if (!header) // memory allocation failed
					throw std::ios::failure("Unable to allocate memory for image header struct");
				
				// parse the image header
				header = ParseImageHeader(buffer);
				_image->ImageHeader = *header;
				
				// check image magic!
				if (!_image->ImageHeader.HasMagic) // no magic word
					throw std::ios::failure("Image has bad magic word");

				// load the common key
				if (_image->ImageHeader.IsWii)
				{
					LoadKey(_image->ImageHeader.IsKoreanKey);
				}
				else
				{
					// the old wii scrubber simply doesn't load any key for gamecube images
				}

				// parse the image
				ParseImage();

				IsLoaded = true;

			}
			else
			{
				// file open failed
				throw std::ios::failure("Unable to open file. Check file exists and is writeable");
			}
		}
		catch (std::ios::failure ex)
		{
			// failed...
			CloseFile();
			free(_image);
			_lastErr = ex.what();
		}
	}

	return IsLoaded;
}

///<summary>
/// Closes the Wii ISO
///<returns>True if the ISO was successfully closed. Otherwise false</returns>
///</summary>
bool Disc::CloseFile()
{
	bool retVal = false;
		try
		{
			fclose(_image->File);
			//free(_image); -- we dont want to free the image. The user should still be able to close the file but still work with some of the loaded settings
			retVal = true;
		}
		catch (std::ios::failure ex) // i dont think this will ever catch anything now fstream class isnt being used. Wont harm to leave it here anyway
		{
			// failed...
			_lastErr = ex.what();
			retVal = false;
		}
	
	return retVal;
}

///<summary>
/// Reads data from the iso into the buffer pointer
///<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
///<param name="size">The amount of data to read in bytes</param>
///<param name="offset">The offset at which to being the read operation</param>
///<param name="markUsed">Indicates whether the clusters being read should be marked as used (eg not empty). Defaults to True</param>
///<returns>The number of bytes read. Returns -1 if an error occurs</returns>
///</summary>
int Disc::Read (unsigned char * buffer, size_t size, u64 offset, bool markUsed)
{

	// open the file
	_image->File = fopen(_isoFileName.c_str(), "rb");

	if (_image->File != NULL)
	{
		size_t byteCount;
		u64 nSeek;
		
		// seek to the correct offset
		nSeek = fseeko64(_image->File, offset + _image->DiscOffset, SEEK_SET);
		//nSeek = fseeko64(_image->File, 262144, SEEK_SET);
		#if defined (__GNUC__) && defined(__unix__)
			// fseeko64 in unix always returns 0 on success so we need to use ftell to get the file position
			nSeek = ftello64(_image->File);
		#endif
		
		// check the seek was successful
		if (nSeek==-1) 
		{
			// seek error
			_lastErr = "Cannot read data from image. Seek error";
			return -1;
		}
		
		if (markUsed)
		{
			// mark the relevant clusters as used
			MarkAsUsed(offset, size);
		}

		// read the data
		byteCount = fread(buffer, 1, size, _image->File);

		// close the file
		Disc::CloseFile();

		if (byteCount == size)
		{
				return byteCount;
		}
		else
		{
			_lastErr = "Read Error. Read operation failed or end of file reached";
			return -1;
		}
	}
	else
	{
		_lastErr = "Unable to open file. Check file exists and is writeable";
		return -1;
	}
}




///<summary>
/// Marks the clusters between nOffset and nOffset + nSize as used (eg not empty)
///<param name="nOffset">The starting position</param>
///<param name="nOffset">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
///<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
///</summary>
int Disc::MarkAsUsed(u64 nOffset, u64 nSize)
{
	int retVal = 0;
	u64 nStartValue = nOffset;
	u64 nEndValue = nOffset + nSize;

	while((nStartValue < nEndValue)&&
		  (nStartValue < (_image->ImageSize)))
	{
		if (_image->FreeClusterTable[nStartValue / (u64)(0x8000)] != 1)
		{
			_image->FreeClusterTable[nStartValue / (u64)(0x8000)] = 1;
		}
		nStartValue = nStartValue + ((u64)(0x8000));
		retVal++;
	}

	return retVal;
}

///<summary>
/// Parses the raw header data in inputData in a part_header structure pointed to by header
///<param name="inputData">Pointer to the data to be parsed</param>
///<returns>Pointer to a part_header structure</returns>
///</summary>
struct part_header * Disc::ParseImageHeader(u8 * inputData)
{
	// initialise the return value pointer
	struct part_header * header;
	header = (struct part_header *) (malloc (sizeof (struct part_header)));
	memset(header, 0, sizeof (struct part_header));

	// populate the struct
	header->DiscId = inputData[0];
	header->Gamecode[0] = inputData[1];
	header->Gamecode[1] = inputData[2];
	header->Region = inputData[3];
	header->Publisher[0] = inputData[4];
	header->Publisher[1] = inputData[5];
	header->HasMagic = (be32 (&inputData[0x18]) == 0x5d1c9ea3)||(be32(&inputData[0x1c])==0xc2339f3d);
	header->IsGC = (be32(&inputData[0x1c])==0xc2339f3d);
	header->IsWii = (be32 (&inputData[0x18]) == 0x5d1c9ea3);
	strncpy(header->GameTitle, (char *) (&inputData[0x20]), 0x60);
	header->DolOffset = be32 (&inputData[0x420]);
	header->FstOffset = be32 (&inputData[0x424]);
	header->FstSize = be32 (&inputData[0x428]);

	if (header->IsWii)
	{
		header->DolOffset *= 4;
		header->FstOffset *= 4;
		header->FstSize *= 4;

		// Now check for Korean disc
		header->IsKoreanKey = (inputData[0x1f1]==1);
	}

	return header;
}

///<summary>
/// Loads the the correct common key for the image. The old WiiScrubber
/// used to load the key from a key.bin file, however since there is no
/// strong evidence that distrbuting the key is illegal, and since everyone
/// has it now anyway, it is embedded in this method. Just to be on the safe
/// side, the actual key is not embeded, instead XORd values are stored
///<param name="inputData">Pointer to the data to be parsed</param>
///<returns>Pointer to a part_header structure</returns>
///</summary>
void Disc::LoadKey(bool korean)
{
	u8 CommonKeyXor[16];
	u8 KorKeyXor[16];
	u8 WiiDLKey[16];
	u8 LoadedKey[16];

	// this is a custom key used by WiiDL to XOR against the common keys
	WiiDLKey[0] = 0x31;
	WiiDLKey[1] = 0x62;
	WiiDLKey[2] = 0x64;
	WiiDLKey[3] = 0x62;
	WiiDLKey[4] = 0x79;
	WiiDLKey[5] = 0x66;
	WiiDLKey[6] = 0x6A;
	WiiDLKey[7] = 0x6D;
	WiiDLKey[8] = 0x30;
	WiiDLKey[9] = 0x33;
	WiiDLKey[10] = 0x43;
	WiiDLKey[11] = 0x61;
	WiiDLKey[12] = 0x54;
	WiiDLKey[13] = 0x5A;
	WiiDLKey[14] = 0x23;
	WiiDLKey[15] = 0x2D;

	// The XOR values of the standard common key
	CommonKeyXor[0] = 0xDA;
	CommonKeyXor[1] = 0x86;
	CommonKeyXor[2] = 0x4E;
	CommonKeyXor[3] = 0x40;
	CommonKeyXor[4] = 0x27;
	CommonKeyXor[5] = 0xE3;
	CommonKeyXor[6] = 0xF9;
	CommonKeyXor[7] = 0x89;
	CommonKeyXor[8] = 0x78;
	CommonKeyXor[9] = 0xEA;
	CommonKeyXor[10] = 0x86;
	CommonKeyXor[11] = 0x24;
	CommonKeyXor[12] = 0x27;
	CommonKeyXor[13] = 0xDB;
	CommonKeyXor[14] = 0x89;
	CommonKeyXor[15] = 0xDA;

	// the XOR values of the korean common key
	KorKeyXor[0] = 0xB9;
	KorKeyXor[1] = 0x3E;
	KorKeyXor[2] = 0x65;
	KorKeyXor[3] = 0xF4;
	KorKeyXor[4] = 0xD3;
	KorKeyXor[5] = 0x82;
	KorKeyXor[6] = 0xB7;
	KorKeyXor[7] = 0xA7;
	KorKeyXor[8] = 0x6B;
	KorKeyXor[9] = 0x18;
	KorKeyXor[10] = 0x78;
	KorKeyXor[11] = 0xDF;
	KorKeyXor[12] = 0x9D;
	KorKeyXor[13] = 0x97;
	KorKeyXor[14] = 0x12;
	KorKeyXor[15] = 0xA4;

	for (int i = 0; i < 16; i++)
	{
		LoadedKey[i] = (korean ? WiiDLKey[i]^KorKeyXor[i] : WiiDLKey[i]^CommonKeyXor[i]);
	}
	memset(&_image->CommonKey, 0, sizeof(AES_KEY));
	AES_set_decrypt_key (LoadedKey, 128, &_image->CommonKey);
}


///<summary>
/// Parses the image file data into the image structure
///</summary>
void Disc::ParseImage()
{
	u8 buffer[0x440];
	u8 *fst;
	u32 i;
	u8 j, valid, nvp;
	u32 nfiles;


	if (_image->ImageHeader.IsWii)
	{
		ParsePartitions();
	}
	else
	{
		_image->Partitions = (struct partition *)malloc (sizeof (struct partition));
		memset(&_image->Partitions[0], 0, sizeof (struct partition));
		_image->PartitionCount = 1;
		_image->PrimaryCount = 1;
		_image->SecondaryCount = 0;
		_image->TertiaryCount = 0;
		_image->QuaternaryCount = 0;
		_image->PrimaryTblOffset = 0;
		_image->SecondaryTblOffset = 0;
		_image->TertiaryTblOffset = 0;
		_image->QuaternaryTblOffset = 0;
		_image->Partitions[0].Type = PART_DATA;
		_image->Partitions[0].DataSize = 1459978240;
		_image->Partitions[0].DataOffset = 0;
		_image->Partitions[0].Offset = 0;
		_image->Partitions[0].IsEncrypted = false;
	}
}

///<summary>
// Parses the partition information into the Partitions structure
///<returns>The number of partitions loaded</returns>
///</summary>
int Disc::ParsePartitions()
{
	int retVal = 0;
	u8 buffer[32];
    u32 i = 0;
    u8 iv[16];
	
	// clear out the old memory allocated
	if (_image->Partitions!=NULL)
	{
		free (_image->Partitions);
		_image->Partitions = NULL;
	}
	// read the partition info
	Disc::Read(buffer, 32, 0x40000);
	// set the partition counts based on the data read
	_image->PrimaryCount = be32 (&buffer[0]);
	_image->SecondaryCount = be32 (&buffer[8]);
	_image->TertiaryCount =  be32 (&buffer[16]);
	_image->QuaternaryCount =  be32 (&buffer[24]);
	_image->PartitionCount = _image->PrimaryCount + _image->SecondaryCount + _image->TertiaryCount + _image->QuaternaryCount + _image->PartitionCount;

	// set the partition offsets
	_image->PrimaryTblOffset = u64 (be32 (&buffer[4])) * ((u64)(4));
	_image->SecondaryTblOffset = (u64 )(be32 (&buffer[12])) * ((u64) (4));
	_image->TertiaryTblOffset = (u64 )(be32 (&buffer[20])) * ((u64) (4));
	_image->QuaternaryTblOffset = (u64 )(be32 (&buffer[28])) * ((u64) (4));

	_image->Partitions = (struct partition *) malloc(_image->PartitionCount * sizeof (struct partition));
	if (0!=_image->PartitionCount)
	{
		memset (_image->Partitions, 0, _image->PartitionCount * sizeof (struct partition));
	}
	
	u64 nOffset = 0;
	u32 nCount;
	for (int z = 0; z < 4; ++z) // loop through each partition group (primary, secondary, etc)
	{
			// set the partition counts and offsets based on the current group
			switch(z)
			{
			case 0:
				nOffset = _image->PrimaryTblOffset;
				nCount = _image->PrimaryCount;
				break;
			case 1:
				nOffset = _image->SecondaryTblOffset;
				nCount = _image->SecondaryCount;
				break;
			case 2:
				nOffset = _image->TertiaryTblOffset;
				nCount = _image->TertiaryCount;
				break;
			case 3:
				nOffset = _image->QuaternaryTblOffset;
				nCount = _image->QuaternaryCount;
				break;
			}

			for (unsigned int x = 0; x < nCount; x++) // loop through each partition in the current group
			{
				// read the partition type
				Read(buffer, 8, nOffset + x * 8);

				switch (be32 (&buffer[4]))
				{
				case 0:
					_image->Partitions[i].Type = PART_DATA;
					break;
					
				case 1:
					_image->Partitions[i].Type = PART_UPDATE;
					break;
					
				case 2:
					_image->Partitions[i].Type = PART_INSTALLER;
					break;
					
				default:
					_image->Partitions[i].Type = PART_VC;
					_image->Partitions[i].ChannelId[0] = buffer[4];
					_image->Partitions[i].ChannelId[1] = buffer[5];
					_image->Partitions[i].ChannelId[2] = buffer[6];
					_image->Partitions[i].ChannelId[3] = buffer[7];
					break;
				}

				_image->Partitions[i].Offset = (u64)(be32 (buffer)) * ((u64)(4));

				// mark the block as used
				MarkAsUsed(_image->Partitions[i].Offset, 0x8000);

				Read(buffer, 8, _image->Partitions[i].Offset + 0x1c);

				if (be32(&buffer[0x0])==0xc2339f3d)
				{
					// Gamecube partition
					_image->Partitions[i].IsEncrypted = 0;
					_image->Partitions[i].DataOffset = 0;
					_image->Partitions[i].Header.IsGC = true;
	                Read(buffer, 8, _image->Partitions[i].Offset + 0x438);
					_image->Partitions[i].DataSize = be32(&buffer[0]);
					_image->Partitions[i].DataOffset = 0;
				}
				else
				{
					Read(buffer, 8, _image->Partitions[i].Offset + 0x2b8);
					_image->Partitions[i].DataOffset = (u64)(be32 (buffer)) << 2;
					_image->Partitions[i].DataSize = (u64)(be32 (&buffer[4])) << 2;
					
					// now get the H3 offset
					Read(buffer, 4, _image->Partitions[i].Offset + 0x2b4);
					_image->Partitions[i].H3Offset = (u64)(be32 (buffer)) << 2 ;
					
					TmdLoad (i);

					// TODO: Finish this method
				}

			}
	}

	return retVal;
}

///<summary>
// Loads the TMD (Title Metadata) info for the give partition number
///<param name="inputData">Pointer to the data to be parsed</param>
///</summary>
void Disc::TmdLoad(u32 partNo)
{
	struct tmd * tmd;
    u32 tmdOffset, tmdSize;
    enum tmd_sig sig = SIG_UNKNOWN;

    u64 off, certSize, certOff;
    u8 buffer[64];
    u16 i, s;

    off = _image->Partitions[partNo].Offset;
    Read(buffer, 16, off + 0x2a4);

    tmdSize = be32 (buffer);
    tmdOffset = be32 (&buffer[4]) * 4;
    certSize = be32 (&buffer[8]);
    certOff = be32 (&buffer[12]) * 4;

	if (0==tmdSize)
	{
		return;
	}

	off += tmdOffset;

    Read(buffer, 4, off);
    off += 4;

    switch (be32 (buffer)) 
	{
		case 0x00010001:
            sig = SIG_RSA_2048;
            s = 0x100;
            break;

		case 0x00010000:
            sig = SIG_RSA_4096;
            s = 0x200;
            break;
    }

    if (sig == SIG_UNKNOWN)
            return;

	tmd = (struct tmd *) malloc (sizeof (struct tmd));
    memset (tmd, 0, sizeof (struct tmd));

    tmd->SigType = sig;

	_image->Partitions[partNo].Tmd = tmd;
	_image->Partitions[partNo].TmdOffset = tmdOffset;
	_image->Partitions[partNo].TmdSize = tmdSize;

	_image->Partitions[partNo].CertOffset = certOff;
	_image->Partitions[partNo].CertSize = certSize;

    tmd->Sig = (unsigned char *) malloc (s);
    Read(tmd->Sig, s, off);
    off += s;
    
    off = ROUNDUP64B(off);

	Read((unsigned char *)&tmd->Issuer[0], 0x40, off);
    off += 0x40;

    Read(buffer, 26, off);
    off += 26;

	tmd->Version = buffer[0];
	tmd->CaCrlVersion = buffer[1];
	tmd->SignerCrlVersion = buffer[2];

	tmd->SysVersion = be64 (&buffer[4]);
	tmd->TitleId = be64 (&buffer[12]);
	tmd->TitleType = be32 (&buffer[20]);
	tmd->GroupId = be16 (&buffer[24]);

    off += 62;

	Read(buffer, 10, off);
    off += 10;

	tmd->AccessRights = be32 (buffer);
	tmd->TitleVersion = be16 (&buffer[4]);
	tmd->NumContents = be16 (&buffer[6]);
	tmd->BootIndex = be16 (&buffer[8]);

    off += 2;

	if (tmd->NumContents < 1)
            return;

	tmd->Contents = (struct tmd_content *) malloc (sizeof (struct tmd_content) * tmd->NumContents);

	for (i = 0; i < tmd->NumContents; ++i) // loop through each TMD content
	{ 
		Read(buffer, 0x30, off);
		off += 0x30;

		tmd->Contents[i].Cid = be32 (buffer);
		tmd->Contents[i].Index = be16 (&buffer[4]);
		tmd->Contents[i].Type = be16 (&buffer[6]);
		tmd->Contents[i].Size = be64 (&buffer[8]);
		memcpy (tmd->Contents[i].Hash, &buffer[16], 20);
	}

	return;
}
