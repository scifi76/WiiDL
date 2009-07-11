
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
	ImageFileName = IsoFileName;

	_blankSector = (unsigned char *) malloc(0x8000);
	memset(_blankSector, 0xFF, 0x8000);

	_blankSector0 = (unsigned char *) malloc(0x8000);
	memset (_blankSector0, 0, 0x8000);

	_lastErr = "";
}

///<summary>
/// Destructor. Disposes of the Disc object
///</summary>
Disc::~Disc()
{
	// Close the ISO file if it's open
	CloseFile();

	
	free(Image->FreeClusterTable);
	free(&Image->ImageHeader);
	free(Image->File);
	free(Image);
	free(_blankSector);
	free(_blankSector0);
}

///<summary>
/// Gets the last error message that occured
///<returns>The most recent error message</returns>
///</summary>
string Disc::GetLastError()
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
			fIsoFile = fopen(ImageFileName.c_str(), mode.c_str());

			//check the file opened ok
			if (fIsoFile != NULL)
			{
				// check if this is a devkitimage (.RVM)
				isoExtension = Utils::StringToUpper(Utils::GetFileExtension(ImageFileName));
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
				
				// get the image info and store it in Image
				struct part_header * header;
		        u8 buffer[0x440];
				// allocate the memory
				Image = (struct image_file *) malloc (sizeof (struct image_file));
		
				if (!Image) // memory allocation failed
					throw std::ios::failure("Unable to allocate memory for image file struct");

				memset (Image, 0, sizeof (struct image_file));
				Image->File = fIsoFile; // add the file pointer to the image structure

				// close the file (for now)
				Disc::CloseFile();
				
				// add the read only flag to the struct
				Image->IsReadOnly = readOnly;

				// set the image size
				Image->ImageSize = imageSize;

				// allocate the FreeClusterTable
				Image->FreeClusterTable = (unsigned char *) malloc(imageSize);
				//set all clusters to free for now
				memset(Image->FreeClusterTable, 0, (Image->ImageSize / (u64)(0x8000)) *2L);
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
				Image->ImageHeader = *header;
				
				// check image magic!
				if (!Image->ImageHeader.HasMagic) // no magic word
					throw std::ios::failure("Image has bad magic word");

				// load the common key
				if (Image->ImageHeader.IsWii)
				{
					LoadKey(Image->ImageHeader.IsKoreanKey);
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
			free(Image);
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
			fclose(Image->File);
			//free(Image); -- we dont want to free the image. The user should still be able to close the file but still work with some of the loaded settings
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
u64 Disc::Read (unsigned char * buffer, size_t size, u64 offset, bool markUsed)
{

	// open the file
	Image->File = fopen(ImageFileName.c_str(), "rb");

	if (Image->File != NULL)
	{
		size_t byteCount;
		u64 nSeek;
		
		// seek to the correct offset
		nSeek = fseeko64(Image->File, offset + Image->DiscOffset, SEEK_SET);
		//nSeek = fseeko64(Image->File, 262144, SEEK_SET);
		#if defined (__GNUC__) && defined(__unix__)
			// fseeko64 in unix always returns 0 on success so we need to use ftell to get the file position
			nSeek = ftello64(Image->File);
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
		byteCount = fread(buffer, 1, size, Image->File);

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
		_lastErr = "Unable to open file. Check file exists and is readable";
		return -1;
	}
}

///<summary>
/// Reads data from the specified partition, decrypting it if necessary
///<param name="buffer">A pointer to a char buffer that the data will be stored in</param>
///<param name="size">The amount of data to read in bytes</param>
///<param name="partNo">The partition to read from</param>
///<param name="offset">The offset (from the beginning of the partition) at which to being the read operation</param>
///<returns>The number of bytes read. Returns -1 if an error occurs</returns>
///</summary>
u64 Disc::ReadFromPartition(unsigned char * buffer, size_t size, u32 partNo, u64 offset) 
{
        u32 block = (u32)(offset / (u64)(0x7c00));
        u32 cacheOffset = (u32)(offset % (u64)(0x7c00));
        u32 cacheSize;
        unsigned char * dst = buffer;

		if (!Image->Partitions[partNo].IsEncrypted)
		{
			// just a plain read of the data
			return Read(buffer, size, Image->Partitions[partNo].Offset + offset);
		}
		else
		{
			while (size) 
			{
				if (DecryptPartitionBlock(partNo, block))
				{
					return (dst - buffer);
				}
				else
				{
					cacheSize = size;
					if (cacheSize + cacheOffset > 0x7c00)
					{
						cacheSize = 0x7c00 - cacheOffset;
					}

					memcpy(dst, Image->Partitions[partNo].Cache + cacheOffset, cacheSize);
					dst += cacheSize;
					size -= cacheSize;
					cacheOffset = 0;
				}
				block++;
			}
			return dst - buffer;
		}
}

///<summary>
/// Reads and decrypts a block of data from the specified partition
///<param name="partNo">The partition to read from</param>
///<param name="block">The block number to read</param>
///<returns>The 0 on success. Returns -1 if an error occurs</returns>
///</summary>
int Disc::DecryptPartitionBlock(u32 partNo, u32 block) 
{
	// have we already decrypted and cached this block?
	if (block == Image->Partitions[partNo].CachedBlock)
	{
		// yes, just return success
		return 0;
	}
	else
	{
		if (Read(Image->Partitions[partNo].DecBuffer, 0x8000, Image->Partitions[partNo].Offset + Image->Partitions[partNo].DataOffset + (u64)(0x8000) * (u64)(block)) != 0x8000)
		{
			// error
			return -1;
        }

		AES_cbc_encrypt(&Image->Partitions[partNo].DecBuffer[0x400],
                         Image->Partitions[partNo].Cache, 0x7c00,
						 &Image->Partitions[partNo].Key,
                         &Image->Partitions[partNo].DecBuffer[0x3d0], AES_DECRYPT);

        Image->Partitions[partNo].CachedBlock = block;

        return 0;
	}
}



///<summary>
/// Marks the clusters between nOffset and nOffset + nSize as used (eg not empty)
///<param name="offset">The starting position</param>
///<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
///<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
///</summary>
int Disc::MarkAsUsed(u64 offset, u64 size)
{
	int retVal = 0;
	u64 startValue = offset;
	u64 endValue = offset + size;

	while((startValue < endValue)&&
		  (startValue < (Image->ImageSize)))
	{
		if (Image->FreeClusterTable[startValue / (u64)(0x8000)] != 1)
		{
			Image->FreeClusterTable[startValue / (u64)(0x8000)] = 1;
		}
		startValue = startValue + ((u64)(0x8000));
		retVal++;
	}

	return retVal;
}

///<summary>
/// Marks the clusters within a partition between nOffset and nOffset + nSize as used (eg not empty). Takes into account whether the data is encrypted and if so accomodates for the extra 1024 bytes of crypto data per cluster
///<param name="partOffset">The starting position</param>
///<param name="offset">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
///<param name="size">The amount of data in bytes to mark as used (will round to the nearset sector)</param>
///<returns>The number of clusters marked as used. 1 Cluster = 32768 bytes (32k)</returns>
///</summary>
int Disc::MarkAsUsedCrypto(u64 partOffset, u64 offset, u64 size, bool isEncrypted)
{
	u64 tempOffset;
	u64 tempSize;
	
	if (isEncrypted)
	{
		// the offset and size relate to the decrypted file so.........
		// we need to change the values to accomodate the 0x400 bytes of crypto data
		
		tempOffset = offset / (u64)(0x7c00);
		tempOffset = tempOffset * ((u64)(0x8000));
		tempOffset += partOffset;
		
		tempSize = size / (u64)(0x7c00);
		tempSize = (tempSize + 1) * ((u64)(0x8000));
		
		// add on the offset in the first nblock for the case where data straddles blocks
		
		tempSize += offset % (u64)(0x7c00);
	}
	else
	{
		// unencrypted - we use the actual offsets
		tempOffset = partOffset + offset;
		tempSize = size;
	}
	return MarkAsUsed(tempOffset, tempSize);

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
	memset(&Image->CommonKey, 0, sizeof(AES_KEY));
	AES_set_decrypt_key (LoadedKey, 128, &Image->CommonKey);
}


///<summary>
/// Parses the image file data into the image structure
///<returns>0 = Success, -1 = Failure</returns>
///</summary>
int Disc::ParseImage()
{
	u8 buffer[0x440];
	u8 * fst;
	u32 i;
	u8 j, valid, nvp;
	u32 nfiles;


	if (Image->ImageHeader.IsWii)
	{
		ParsePartitions();
	}
	else
	{
		Image->Partitions = (struct partition *)malloc (sizeof (struct partition));
		memset(&Image->Partitions[0], 0, sizeof (struct partition));
		Image->PartitionCount = 1;
		Image->PrimaryCount = 1;
		Image->SecondaryCount = 0;
		Image->TertiaryCount = 0;
		Image->QuaternaryCount = 0;
		Image->PrimaryTblOffset = 0;
		Image->SecondaryTblOffset = 0;
		Image->TertiaryTblOffset = 0;
		Image->QuaternaryTblOffset = 0;
		Image->Partitions[0].Type = PART_DATA;
		Image->Partitions[0].DataSize = 1459978240;
		Image->Partitions[0].DataOffset = 0;
		Image->Partitions[0].Offset = 0;
		Image->Partitions[0].IsEncrypted = false;
	}

	nvp = 0;
	for (i = 0; i < Image->PartitionCount; ++i)
	{
		if (!ReadFromPartition(buffer, 0x440, i, 0)) 
		{
			//_lastErr = "Failed to parse image. Error while reading from partition";
			return -1;
		}
		
		valid = 1;
		for (j = 0; j < 6; ++j) {
			if (!isprint (buffer[j])) {
				valid = 0;
				break;
			}
		}

		if (valid)
		{
			// valid partition
			nvp++;
			Image->Partitions[i].Header = *ParseImageHeader(buffer);
			Image->Partitions[i].IsValid = true;

			if (Image->Partitions[i].Type!=PART_UNKNOWN)
			{
				if (Image->Partitions[i].Header.IsWii)
				{
					AddFileToPart("partition.bin", "\\", &Image->Partitions[i], 0x0, Image->Partitions[i].DataOffset);
					MarkAsUsed(Image->Partitions[i].Offset, Image->Partitions[i].DataOffset);
				}
				// add on the boot.bin
				AddFileToPart("boot.bin", "\\", &Image->Partitions[i], Image->Partitions[i].DataOffset, (u64)0x440);
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset, 0, (u64)0x440, Image->Partitions[i].IsEncrypted);
				
				// add on the bi2.bin
				AddFileToPart("bi2.bin", "\\", &Image->Partitions[i], Image->Partitions[i].DataOffset + 0x440, (u64)0x2000);
			}

			ReadFromPartition(buffer, 9, i, 0x2440 + 0x14);
			Image->Partitions[i].AppldrSize = be32 (buffer) + be32 (&buffer[4]);

			if (Image->Partitions[i].AppldrSize > 0)
			{
				Image->Partitions[i].AppldrSize += 32;
				AddFileToPart("apploader.img", "\\", &Image->Partitions[i], Image->Partitions[i].DataOffset + 0x2440, Image->Partitions[i].AppldrSize);
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset,	0x2440,	Image->Partitions[i].AppldrSize, Image->Partitions[i].IsEncrypted);
			}

			if (Image->Partitions[i].Header.DolOffset > 0)
			{
				ReadFromPartition(buffer, 0x100, i, Image->Partitions[i].Header.DolOffset);
				Image->Partitions[i].Header.DolSize = get_dol_size (buffer);
				
				// now check for error condition with bad main.dol
				if (Image->Partitions[i].Header.DolSize >= Image->Partitions[i].DataSize)
				{
					// almost certainly an error as it's bigger than the partition
					_lastErr = "main.dol read error. Size is larger than partition size";
					Image->Partitions[i].Header.DolSize = 0;
					
				}
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset, Image->Partitions[i].Header.DolOffset, Image->Partitions[i].Header.DolSize, Image->Partitions[i].IsEncrypted);
				
				AddFileToPart("main.dol", "\\", &Image->Partitions[i], Image->Partitions[i].DataOffset + Image->Partitions[i].Header.DolOffset, Image->Partitions[i].Header.DolSize);
				
			} 
			else
			{
				_lastErr = "Missing main.dol in partition";
			}

			if (Image->Partitions[i].IsEncrypted)
			{
				// Now add the TMD.BIN and cert.bin files - as these are part of partition.bin
				// we don't need to mark them as used
				AddFileToPart("tmd.bin", "\\", &Image->Partitions[i], Image->Partitions[i].TmdOffset, Image->Partitions[i].TmdSize);
				
				AddFileToPart("cert.bin", "\\", &Image->Partitions[i], Image->Partitions[i].CertOffset, Image->Partitions[i].CertSize);
				
				AddFileToPart("h3.bin", "\\", &Image->Partitions[i], Image->Partitions[i].H3Offset, (u64)0x18000);
				MarkAsUsedCrypto(Image->Partitions[i].Offset, Image->Partitions[i].H3Offset, (u64)0x18000, false);
				
				AddFileToPart("ticket.bin", "\\", &Image->Partitions[i], Image->Partitions[i].Offset, (u64)0x2a4);
				MarkAsUsedCrypto(Image->Partitions[i].Offset, 0, (u64)0x2a4, false);
			}
			
			if (Image->Partitions[i].Header.FstOffset > 0 && Image->Partitions[i].Header.FstSize > 0)
			{
				AddFileToPart("fst.bin", "\\", &Image->Partitions[i], Image->Partitions[i].DataOffset + Image->Partitions[i].Header.FstOffset, Image->Partitions[i].Header.FstSize);
				MarkAsUsedCrypto(Image->Partitions[i].Offset +Image->Partitions[i].DataOffset, Image->Partitions[i].Header.FstOffset, Image->Partitions[i].Header.FstSize, Image->Partitions[i].IsEncrypted);
				
				fst = (u8 *) (malloc ((u32)(Image->Partitions[i].Header.FstSize)));
				if (ReadFromPartition(fst, (u32)(Image->Partitions[i].Header.FstSize), i, Image->Partitions[i].Header.FstOffset) != Image->Partitions[i].Header.FstSize)
				{
					Image->Partitions[i].IsValid = false;
					_lastErr = "Error reading fst.bin";
					free (fst);
					//return -1; -- I think we should break instead to continue with the OTHER partitions
					break;
				}
				
				nfiles = be32 (fst + 8);
				
				if (12 * nfiles > Image->Partitions[i].Header.FstSize)
				{
					Image->Partitions[i].IsValid = false;
					_lastErr = "fst.bin is invalid";
					break;
				} 
				else 
				{
					ParseFst(fst, (char *) (fst + 12 * nfiles), "\\", 0, NULL, i);
				}
				
				//free (fst);
			} else
			{
				Image->Partitions[i].IsValid = false;
				_lastErr = "Missing fst.bin in partition!";
				break;
				
			}

		}
		else
		{
			// bad partition
			Image->Partitions[i].IsValid = false;
			_lastErr = "Unable to read partition";
			break;
			
		}

	}
}

///<summary>
/// Parses a partition's fst.bin file as passed into fst and adds all of it's files into the partitions Files collection
///<param name="fst">Pointer to the fst.bin data</param>
///<param name="names">??</param>
///<param name="currentDir">The current directory. usually "\"</param>
///<param name="i">??</param>
///<param name="tree">??</param>
///<param name="tree">??</param>
///<param name="partNo">The partiton number to parse</param>
///<returns>The number of directories or files parsed</returns>
///</summary>
u32 Disc::ParseFst(u8 * fst, const char * names, const char * currentDir, u32 i, struct tree * tree, u32 partNo)
{
	u64 offset;
    u32 size;
    const char * name;
    u32 j;

    name = names + (be32 (fst + 12 * i) & 0x00ffffff);
    size = be32 (fst + 12 * i + 8);

    if (i == 0) // root directory
	{
		// directory so need to go through the directory entries
        for (j = 1; j < size; )
		{
			j = ParseFst(fst, names, currentDir, j, tree, partNo);
		}
		return size;
	}

    if (fst[12 * i]) 
	{
		// directory
		/*char * dirName = strcat(currentDir, name);
		dirName = strcat(dirName, "\\");*/
  
		string dirName = currentDir;
		dirName.append(name);
		dirName.append("\\");
        for (j = i + 1; j < size; )
			j = ParseFst (fst, names, dirName.c_str(), j, NULL, partNo);

		// now remove the directory name we just added
		//m_csText = m_csText.Left(m_csText.GetLength()-strlen(name) - 1);
        return size;
    }
	else
	{
		//file
		offset = be32(fst + 12 * i + 4);
		if (Image->Partitions[partNo].Header.IsWii)
		{
			offset *= 4;
		}
		
		AddFileToPart(name, currentDir, &Image->Partitions[partNo], offset, size);
		MarkAsUsedCrypto(Image->Partitions[partNo].Offset + Image->Partitions[partNo].DataOffset, offset, size, Image->Partitions[partNo].IsEncrypted);

		Image->FileCount ++;
		Image->BytesCount += size;

        return i + 1;
    }
}

///<summary>
/// Creates a partition_file struct and adds it to the Files collection of the partition pointed to by part
/// Note that this method does NOT add a new file into the image
///<param name="fileName">The name of the file</param>
///<param name="part">The partition to add the file to</param>
///<param name="offset">The offset of the file within the partition</param>
///<param name="size">The size of the file</param>
///<returns>Pointer to a part_header structure</returns>
///</summary>
void Disc::AddFileToPart(const char * fileName, const char * directoryName, struct partition * part, u64 offset, u64 size)
{
	// create a new file struct
	struct partition_file * file;
	file = (struct partition_file *) (malloc (sizeof (struct partition_file)));
	memset(file, 0, sizeof (struct partition_file));

	file->FileName = fileName;
	file->Offset = offset;
	file->Size = size;

	file->DirectoryName = directoryName;
	
	part->Files.Add(file);

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
	u8 titleKey[16];
    u8 partitionKey[16];
    u8 iv[16];
	
	// clear out the old memory allocated
	if (Image->Partitions!=NULL)
	{
		free (Image->Partitions);
		Image->Partitions = NULL;
	}
	// read the partition info
	Disc::Read(buffer, 32, 0x40000);
	// set the partition counts based on the data read
	Image->PrimaryCount = be32 (&buffer[0]);
	Image->SecondaryCount = be32 (&buffer[8]);
	Image->TertiaryCount =  be32 (&buffer[16]);
	Image->QuaternaryCount =  be32 (&buffer[24]);
	Image->PartitionCount = Image->PrimaryCount + Image->SecondaryCount + Image->TertiaryCount + Image->QuaternaryCount + Image->PartitionCount;

	// set the partition offsets
	Image->PrimaryTblOffset = u64 (be32 (&buffer[4])) * ((u64)(4));
	Image->SecondaryTblOffset = (u64 )(be32 (&buffer[12])) * ((u64) (4));
	Image->TertiaryTblOffset = (u64 )(be32 (&buffer[20])) * ((u64) (4));
	Image->QuaternaryTblOffset = (u64 )(be32 (&buffer[28])) * ((u64) (4));

	Image->Partitions = (struct partition *) malloc(Image->PartitionCount * sizeof (struct partition));
	if (0!=Image->PartitionCount)
	{
		memset (Image->Partitions, 0, Image->PartitionCount * sizeof (struct partition));
	}
	
	u64 nOffset = 0;
	u32 nCount;
	for (int z = 0; z < 4; ++z) // loop through each partition group (primary, secondary, etc)
	{
			// set the partition counts and offsets based on the current group
			switch(z)
			{
			case 0:
				nOffset = Image->PrimaryTblOffset;
				nCount = Image->PrimaryCount;
				break;
			case 1:
				nOffset = Image->SecondaryTblOffset;
				nCount = Image->SecondaryCount;
				break;
			case 2:
				nOffset = Image->TertiaryTblOffset;
				nCount = Image->TertiaryCount;
				break;
			case 3:
				nOffset = Image->QuaternaryTblOffset;
				nCount = Image->QuaternaryCount;
				break;
			}

			for (unsigned int x = 0; x < nCount; x++) // loop through each partition in the current group
			{
				// read the partition type
				Read(buffer, 8, nOffset + x * 8);

				switch (be32 (&buffer[4]))
				{
				case 0:
					Image->Partitions[i].Type = PART_DATA;
					break;
					
				case 1:
					Image->Partitions[i].Type = PART_UPDATE;
					break;
					
				case 2:
					Image->Partitions[i].Type = PART_INSTALLER;
					break;
					
				default:
					Image->Partitions[i].Type = PART_VC;
					Image->Partitions[i].ChannelId[0] = buffer[4];
					Image->Partitions[i].ChannelId[1] = buffer[5];
					Image->Partitions[i].ChannelId[2] = buffer[6];
					Image->Partitions[i].ChannelId[3] = buffer[7];
					break;
				}

				Image->Partitions[i].Offset = (u64)(be32 (buffer)) * ((u64)(4));

				// mark the block as used
				MarkAsUsed(Image->Partitions[i].Offset, 0x8000);

				Read(buffer, 8, Image->Partitions[i].Offset + 0x1c);

				if (be32(&buffer[0x0])==0xc2339f3d)
				{
					// Gamecube partition
					Image->Partitions[i].IsEncrypted = 0;
					Image->Partitions[i].DataOffset = 0;
					Image->Partitions[i].Header.IsGC = true;
	                Read(buffer, 8, Image->Partitions[i].Offset + 0x438);
					Image->Partitions[i].DataSize = be32(&buffer[0]);
					Image->Partitions[i].DataOffset = 0;
				}
				else
				{
					Read(buffer, 8, Image->Partitions[i].Offset + 0x2b8);
					Image->Partitions[i].DataOffset = (u64)(be32 (buffer)) << 2;
					Image->Partitions[i].DataSize = (u64)(be32 (&buffer[4])) << 2;
					
					// now get the H3 offset
					Read(buffer, 4, Image->Partitions[i].Offset + 0x2b4);
					Image->Partitions[i].H3Offset = (u64)(be32 (buffer)) << 2 ;
					
					if (TmdLoad(i) == NULL)
					{
						//no valid TMD
						Image->Partitions[i].IsEncrypted = 0;
					}
					else
					{
						Image->Partitions[i].IsEncrypted = 1;
						Image->Partitions[i].CachedBlock = 0xffffffff;
						
						memset(titleKey, 0, 16);
						memset(iv, 0, 16);
						
						Read(titleKey, 16, Image->Partitions[i].Offset + 0x1bf);
						Read(iv, 8, Image->Partitions[i].Offset + 0x1dc);
						
						AES_cbc_encrypt(titleKey, partitionKey, 16, &Image->CommonKey, iv, AES_DECRYPT);
						
						memcpy(Image->Partitions[i].TitleKey, partitionKey, 16);
						
						AES_set_decrypt_key (partitionKey, 128, &Image->Partitions[i].Key);
					}
				}
				i++;
			}
	}

	return retVal;
}

///<summary>
// Loads the TMD (Title Metadata) info for the give partition number
///<param name="partNo">The partition number to load the TMD from</param>
///</summary>
tmd * Disc::TmdLoad(u32 partNo)
{
	struct tmd * tmd;
    u32 tmdOffset, tmdSize;
    enum tmd_sig sig = SIG_UNKNOWN;

    u64 off, certSize, certOff;
    u8 buffer[64];
    u16 i, s;

    off = Image->Partitions[partNo].Offset;
    Read(buffer, 16, off + 0x2a4);

    tmdSize = be32 (buffer);
    tmdOffset = be32 (&buffer[4]) * 4;
    certSize = be32 (&buffer[8]);
    certOff = be32 (&buffer[12]) * 4;

	if (tmdSize==0)
	{
		_lastErr = "Zero TMD size";
		return NULL;
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
	{
		_lastErr = "Unknown TMD Signature";
        return NULL;
	}

	tmd = (struct tmd *) malloc (sizeof (struct tmd));
    memset (tmd, 0, sizeof (struct tmd));

    tmd->SigType = sig;

	Image->Partitions[partNo].Tmd = tmd;
	Image->Partitions[partNo].TmdOffset = tmdOffset;
	Image->Partitions[partNo].TmdSize = tmdSize;

	Image->Partitions[partNo].CertOffset = certOff;
	Image->Partitions[partNo].CertSize = certSize;

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
	{
		_lastErr = "Empty TMD Contents";
        return NULL;
	}

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

	return tmd;
}
