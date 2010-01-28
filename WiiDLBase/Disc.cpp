
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
				Image->ImageHeader = header;
				
				// check image magic!
				if (!Image->ImageHeader->HasMagic) // no magic word
					throw std::ios::failure("Image has bad magic word");

				// load the common key
				if (Image->ImageHeader->IsWii)
				{
					LoadKey(Image->ImageHeader->IsKoreanKey);
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


	if (Image->ImageHeader->IsWii)
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
			Image->Partitions[i].Header = ParseImageHeader(buffer);
			Image->Partitions[i].IsValid = true;

			// create the root folder
			partition_folder * rootFolder = new partition_folder;
			rootFolder->FolderName = "\\";

			if (Image->Partitions[i].Type!=PART_UNKNOWN)
			{
				if (Image->Partitions[i].Header->IsWii)
				{
					rootFolder->AddFile("partition.bin", Image->Partitions[i].Offset, Image->Partitions[i].DataOffset, i, -4);
					MarkAsUsed(Image->Partitions[i].Offset, Image->Partitions[i].DataOffset);
				}
				// add on the boot.bin
				rootFolder->AddFile("boot.bin", 0x0, (u64)0x440, i, 0);
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset, 0, (u64)0x440, Image->Partitions[i].IsEncrypted);
				
				// add on the bi2.bin
				rootFolder->AddFile("bi2.bin", 0x440, (u64)0x2000, i, 0);
								MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset, 0x440, (u64)0x2000, Image->Partitions[i].IsEncrypted);
			}

			ReadFromPartition(buffer, 9, i, 0x2440 + 0x14);
			Image->Partitions[i].AppldrSize = be32 (buffer) + be32 (&buffer[4]);

			if (Image->Partitions[i].AppldrSize > 0)
			{
				Image->Partitions[i].AppldrSize += 32;
				rootFolder->AddFile("apploader.img", 0x2440, Image->Partitions[i].AppldrSize, i, -3);
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset,	0x2440,	Image->Partitions[i].AppldrSize, Image->Partitions[i].IsEncrypted);
			}

			if (Image->Partitions[i].Header->DolOffset > 0)
			{
				ReadFromPartition(buffer, 0x100, i, Image->Partitions[i].Header->DolOffset);
				Image->Partitions[i].Header->DolSize = get_dol_size (buffer);
				
				// now check for error condition with bad main.dol
				if (Image->Partitions[i].Header->DolSize >= Image->Partitions[i].DataSize)
				{
					// almost certainly an error as it's bigger than the partition
					_lastErr = "main.dol read error. Size is larger than partition size";
					Image->Partitions[i].Header->DolSize = 0;
					
				}
				MarkAsUsedCrypto(Image->Partitions[i].Offset + Image->Partitions[i].DataOffset, Image->Partitions[i].Header->DolOffset, Image->Partitions[i].Header->DolSize, Image->Partitions[i].IsEncrypted);
				
				rootFolder->AddFile("main.dol", Image->Partitions[i].Header->DolOffset, Image->Partitions[i].Header->DolSize, i, -2);
				
			} 
			else
			{
				_lastErr = "Missing main.dol in partition";
			}

			if (Image->Partitions[i].IsEncrypted)
			{
				// Now add the TMD.BIN and cert.bin files - as these are part of partition.bin
				// we don't need to mark them as used
				rootFolder->AddFile("tmd.bin", Image->Partitions[i].TmdOffset, Image->Partitions[i].TmdSize, i, -5);
				
				rootFolder->AddFile("cert.bin", Image->Partitions[i].CertOffset, Image->Partitions[i].CertSize, i, -6);
				
				rootFolder->AddFile("h3.bin", Image->Partitions[i].H3Offset, (u64)0x18000, i, -7);
				MarkAsUsedCrypto(Image->Partitions[i].Offset, Image->Partitions[i].H3Offset, (u64)0x18000, false);
				
				rootFolder->AddFile("ticket.bin", Image->Partitions[i].Offset, (u64)0x2a4, i, -8);
				MarkAsUsedCrypto(Image->Partitions[i].Offset, 0, (u64)0x2a4, false);
			}
			
			if (Image->Partitions[i].Header->FstOffset > 0 && Image->Partitions[i].Header->FstSize > 0)
			{
				rootFolder->AddFile("fst.bin", Image->Partitions[i].Header->FstOffset, Image->Partitions[i].Header->FstSize, i, -1);
				MarkAsUsedCrypto(Image->Partitions[i].Offset +Image->Partitions[i].DataOffset, Image->Partitions[i].Header->FstOffset, Image->Partitions[i].Header->FstSize, Image->Partitions[i].IsEncrypted);
				
				fst = (u8 *) (malloc ((u32)(Image->Partitions[i].Header->FstSize)));
				if (ReadFromPartition(fst, (u32)(Image->Partitions[i].Header->FstSize), i, Image->Partitions[i].Header->FstOffset) != Image->Partitions[i].Header->FstSize)
				{
					Image->Partitions[i].IsValid = false;
					_lastErr = "Error reading fst.bin";
					free (fst);
					//return -1; -- I think we should break instead to continue with the OTHER partitions
					break;
				}
				
				nfiles = be32 (fst + 8);
				
				if (12 * nfiles > Image->Partitions[i].Header->FstSize)
				{
					Image->Partitions[i].IsValid = false;
					_lastErr = "fst.bin is invalid";
					break;
				} 
				else 
				{
					ParseFst(fst, (char *) (fst + 12 * nfiles), rootFolder, 0, NULL, i);
				}
				
				//free (fst);
			} else
			{
				Image->Partitions[i].IsValid = false;
				_lastErr = "Missing fst.bin in partition!";
				break;
				
			}
			
			Image->Partitions[i].Folders.Add(rootFolder);
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
u32 Disc::ParseFst(u8 * fst, const char * names, partition_folder * currentFolder, u32 i, struct tree * tree, u32 partNo)
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
			j = ParseFst(fst, names, currentFolder, j, tree, partNo);
		}
		return size;
	}

    if (fst[12 * i]) 
	{
		// directory
		/*char * dirName = strcat(currentDir, name);
		dirName = strcat(dirName, "\\");*/
		
		partition_folder * subFolder = new partition_folder;
		subFolder->FolderName = name;

        for (j = i + 1; j < size; )
			j = ParseFst (fst, names, subFolder, j, NULL, partNo);

		// now remove the directory name we just added
		//m_csText = m_csText.Left(m_csText.GetLength()-strlen(name) - 1);

		currentFolder->SubFolders.Add(subFolder);
        return size;
    }
	else
	{
		//file
		offset = be32(fst + 12 * i + 4);
		if (Image->Partitions[partNo].Header->IsWii)
		{
			offset *= 4;
		}
		
		currentFolder->AddFile(name, offset, size, partNo, i);
		MarkAsUsedCrypto(Image->Partitions[partNo].Offset + Image->Partitions[partNo].DataOffset, offset, size, Image->Partitions[partNo].IsEncrypted);

		Image->FileCount ++;
		Image->BytesCount += size;

        return i + 1;
    }
}

///<summary>
/// Deprecated - Files are now children of the Folders collection
/// Creates a partition_file struct and adds it to the Files collection of the partition pointed to by part
/// Note that this method does NOT add a new file into the image
///<param name="fileName">The name of the file</param>
///<param name="part">The partition to add the file to</param>
///<param name="offset">The offset of the file within the partition</param>
///<param name="size">The size of the file</param>
///<returns>Pointer to a part_header structure</returns>
///</summary>
//void Disc::AddFileToPart(const char * fileName, const char * directoryName, struct partition * part, u64 offset, u64 size)
//{
//	// create a new file struct
//	struct partition_file * file;
//	file = (struct partition_file *) (malloc (sizeof (struct partition_file)));
//	memset(file, 0, sizeof (struct partition_file));
//
//	file->FileName = fileName;
//	file->Offset = offset;
//	file->Size = size;
//
//	
//	part->Files.Add(file);
//
//}


///<summary>
/// Creates a partition_folder struct and adds it to the Folders collection of the partition pointed to by part
/// Note that this method does NOT add a new folder into the image
///<param name="folderName">The name of the file</param>
///<param name="part">The partition to add the folder to</param>
///<param name="size">The size of the file</param>
///<returns>Pointer to a part_header structure</returns>
///</summary>
//void Disc::AddFolderToPart(const char * folderName, struct partition * part)
//{
//	struct partition_folder * folder;
//	folder = (struct partition_folder) (malloc (sizeof (struct partition_folder)));
//	memset(folder, 0, sizeof (struct partition_foler));
//
//	folder->FolderName = folderName;
//
//	part->Folders.Add(folder);
//}

///<summary>
/// Parses the partition information into the Partitions structure
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
					Image->Partitions[i].Header->IsGC = true;
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
/// Loads the TMD (Title Metadata) info for the give partition number
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

///<summary>
/// Extracts a file to disk
///<param name="destFilename">The filename and path to extract to</param>
///<param name="file">A reference to the file object to extract</param>
///<param name="decrypt">Whether or not the file should be decrypted when extracted</param>
///</summary>
bool Disc::ExtractFile(const char * destFilename, PartitionFile * file, bool decrypt)
{
	FILE * fOut;
	FILE * fIn;
	try
	{
		if (IsLoaded)
		{
			
			u64 nFileOffset = file->Offset;
			u32 block = (u32)(nFileOffset / (u64)(0x7c00));
			u32 cache_offset = (u32)(nFileOffset % (u64)(0x7c00));
			u64 cache_size;
			u64 nFileSize = file->Size;
			u64 imageSize;

			fOut = fopen(destFilename, "wb");
			fIn = fopen(ImageFileName.c_str(), "rb");
			if (Image->Partitions[file->PartNo].IsEncrypted && decrypt)
			{
				//decrypt and save
				//u64 nFileOffset = Image->Partitions[partNo].Offset + Image->DiscOffset;
				if (fseeko64(fIn, nFileOffset, SEEK_SET) != -1)
				{
					while (nFileSize) 
					{
						if (DecryptPartitionBlock(file->PartNo, block))
						{
							fclose(fOut);
							return false;
						}

						cache_size = nFileSize;
						if (cache_size + cache_offset > 0x7c00)
								cache_size = 0x7c00 - cache_offset;

						if (cache_size!=fwrite(Image->Partitions[file->PartNo].Cache + cache_offset, 1, (u32)(cache_size), fOut))
						{
							//throw std::ios::failure("Error writing to output file");
							//return false;
						}
	 
						// TODO: Raise some sort of event so consumers of the library can track progress
						nFileSize -= cache_size;
						cache_offset = 0;

						block++;
					}
					fclose(fOut);
					return true;
				}
				else
				{
					throw std::ios::failure("Error seeking to correct file offset");
				}
			}
			else
			{
				// TODO: Implement this
				throw std::ios::failure("Not implemented yet");
			}
		}
		else
		{
			throw std::ios::failure("Unable to extract file. ISO must be loaded first");
		}
	}
	catch (std::ios::failure ex)
	{
		fclose(fOut);
		_lastErr = ex.what();
		return false;
	}
	

}


///<summary>
/// Replaces a file on the disc with the file given in inputFilename
///<param name="inputFilename">The path and filename of the file to write to the image</param>
///<param name="PartitionFile">The file to replace</param>
///<param name="encrypt">Whether or not the file should be encrypted when written to the image</param>
///</summary>
bool Disc::ReplaceFile(const char * inputFilename, PartitionFile * file, bool encrypt)
{
	FILE * fIn;
	try	
	{
		if (IsLoaded)
		{
			u8		cTempBuffer[8];
			u8 *	pTMDData = NULL;
			u8 *	pCERTData  = NULL;
			
			u32		nImageSize;	
			u64		nfImageSize;
			u8 *	pBootBin = (unsigned char *) calloc(0x440,1);
			u8 *	pPartData ;
			u64		nFreeSpaceStart;
			u32		nExtraData;
			u32		nExtraDataBlocks;
			int		nFSTReference;

			// create a 64 cluster buffer for the file
			fIn = fopen(inputFilename, "rb");
			
			if (fIn != NULL)
			{
				// get the size of the file we are trying to load
				nfImageSize = fseeko64(fIn, 0L, SEEK_END);
				nImageSize = (u32) nfImageSize;

				// seek back to the start
				fseeko64(fIn, 0L, SEEK_SET);

				// check the file is smaller or the same size as the file we're trying to replace
				if (file->Size >= nImageSize)
				{
					// straightforward file replace

					// are the files the same size?
					if (file->Size != nfImageSize)
					{
						// we have a smaller sized file being put in. Is it a special file?
						if (file->FstRef > 0)
						{
							// normal file. need to modify the FST.BIN for the smaller file
							// allocate memory for the FST.bin
							u8 * pFSTBin = (unsigned char *) calloc((u32)(Image->Partitions[file->PartNo].Header->FstSize), 1);
							
							// read in the FST.bin from the image into pFSTBin
							this->ReadFromPartition(pFSTBin, Image->Partitions[file->PartNo].Header->FstSize, file->PartNo, Image->Partitions[file->PartNo].Header->FstOffset);

							// get the offset of this file within the fst.bin based on the fstRef
							// each file entry in the FST.bin uses 12bytes, so to get to the correct
							// position in fst.bin for file number x, we do x * 12 (0x0c)
							nFSTReference = file->FstRef * 0x0c;
							
							// modify the file size stored in the fst.bin
							Write32(pFSTBin + nFSTReference + 0x08L , nImageSize);

							// write the fst.bin back to the image
							// write out the FST.BIN
							if (WriteData(file->PartNo, Image->Partitions[file->PartNo].Header->FstOffset, Image->Partitions[file->PartNo].Header->FstSize, pFSTBin, NULL))
							{
								if (WriteData(file->PartNo, file->Offset, nImageSize, NULL, fIn))
								{
									// success
								}
								else
								{
									// the methods that caused WriteData to return False will have set _lastErr
									throw std::ios::failure(_lastErr);
								}

							}
							else
							{
								// the methods that caused WriteData to return False will have set _lastErr
								throw std::ios::failure(_lastErr);
							}
						}
						else
						{
							// TODO: Finish this
							// special file
						}
					}
					else
					{
						// TODO: Finish this
						// equal sized file. Check if it's a special file, otherwise just straight replace
					}

				}
				else
				{
					// more complicated file replace. we have to update the FST or boot.bin
					// this will depend on whether the passed index is
					// -ve = Partition data,
					// 0 = given by boot.bin,
					// +ve = normal file

					// TODO: Finish this
				}

			}
			else
			{
				throw std::ios::failure("Failed to open input file");
			}
		}
		else
		{
			throw std::ios::failure("Unable to replace file. ISO must be loaded first");
		}
	}
	catch (std::ios::failure ex)
	{
		fclose(fIn);
		_lastErr = ex.what();
		return false;
	}
}


///<summary>
/// Heavily optimised file write routine so that the minimum number of
/// SHA calculations have to be performed                             
/// We do this by writing in 1 clustergroup per write and calculate the
/// Offset to write the data in the minimum number of chunks           
/// A bit like lumpy chunk packer from the Atari days...
///<param name="partNo">The The partition number the file should be written to</param>
///<param name="offset">The offset at which to write the file</param>
///<param name="in">The data to write</param>
///<param name="fIn">The file from which to read data that will be written (optional)</param>
///</summary>
bool Disc::WriteData(int partNo, u64 offset, u64 size, u8 *in, FILE * fIn)
{
	u32 cluster_start;
	u32 clusters;
	u32 offset_start;

	u64 i;

	u32 nClusterCount;
	u32 nWritten = 0;

	/* Calculate some needed information */
	// the starting cluster for this file offset
	cluster_start = (u32)(offset / (u64)(SIZE_CLUSTER_DATA));
	// the number of clusters needed to store this file size
	clusters = (u32)(((offset + size) / (u64)(SIZE_CLUSTER_DATA)) - (cluster_start - 1));
	// the start offset of the cluster... not sure why this is different to the original file offset
	offset_start = (u32)(offset - (cluster_start * (u64)(SIZE_CLUSTER_DATA)));

	// read the H3 and H4
	this->Read(Image->h3, SIZE_H3, Image->Partitions[partNo].Offset + Image->Partitions[partNo].H3Offset);
	
	
	i = 0;
	nClusterCount = 0;
	
	/* Write clusters */
	while(i < size)
	{
		// now the fun bit as we need to cater for the start position changing as well as the 
		// wrap over 
		if ((0!=((cluster_start+nClusterCount)%64))|| (0!=offset_start))
		{
			// not at the start so our max size is different
			// and also our cluster offset
			nWritten = (NB_CLUSTER_GROUP - (cluster_start%64))* SIZE_CLUSTER_DATA;
			nWritten = nWritten - offset_start;

			// max check
			if (nWritten > size)
			{
				nWritten = (u32)size;
			}

			if (WriteClusters(partNo, cluster_start, in, offset_start, nWritten, fIn) == false)
			{
				return false;
			}
			// round up the cluster count
			nClusterCount = NB_CLUSTER_GROUP - (cluster_start % NB_CLUSTER_GROUP);
		}
		else
		{
			// potentially full block
			nWritten = NB_CLUSTER_GROUP * SIZE_CLUSTER_DATA;

			// max check
			if (nWritten > (size-i))
			{
				nWritten = (u32)(size-i);
			}

			if (WriteClusters(partNo, cluster_start + nClusterCount, in, offset_start, nWritten, fIn) == false)
			{
				return false;
			}
			// we simply add a full cluster block
			nClusterCount = nClusterCount + NB_CLUSTER_GROUP;

		}
		offset_start = 0;
		i += nWritten;
	}
	

	// write out H3 and H4
	if (false==DiscWriteDirect(Image->Partitions[partNo].H3Offset + Image->Partitions[partNo].Offset, Image->h3, SIZE_H3))
	{
		return false;
	}
	
	/* Calculate H4 */
	sha1(Image->h3, SIZE_H3, Image->h4);

	/* Write H4 */
	if (false==DiscWriteDirect(Image->Partitions[partNo].TmdOffset + OFFSET_TMD_HASH + Image->Partitions[partNo].Offset, Image->h4, SIZE_H4))
	{
		return false;
	}

	// sign it
	wii_trucha_signing(partNo);

	return true;
}

////////////////////////////////////////////////////////////
// Inverse of the be32 function - writes a 32 bit value   //
// high to low                                            //
////////////////////////////////////////////////////////////
void Disc::Write32( u8 *p, u32 nVal)
{
	p[0] = (nVal >> 24) & 0xFF;
	p[1] = (nVal >> 16) & 0xFF;
	p[2] = (nVal >>  8) & 0xFF;
	p[3] = (nVal      ) & 0xFF;

}

bool Disc::WriteClusters(int partNo, u32 cluster, u8 * in, u32 clusterOffset, u32 bytesToWrite, FILE * fIn)
{
u8 h0[SIZE_H0];
	u8 h1[SIZE_H1];
	u8 h2[SIZE_H2];

	u8 *data;
	u8 *header;
	u8 *title_key;

	u8 iv[16];
	
	u32 group,
		subgroup,
		f_cluster,
		nb_cluster,
		pos_cluster,
		pos_header;
	
	u64 offset;

	u32 i;
	int j;

	int ret = 0;

	int nClusters = 0;

	/* Calculate cluster group and subgroup */
	group = cluster / NB_CLUSTER_GROUP;
	subgroup = (cluster % NB_CLUSTER_GROUP) / NB_CLUSTER_SUBGROUP;

	/* First cluster in the group */
	f_cluster = group * NB_CLUSTER_GROUP;

	/* Get number of clusters in this group */
	nb_cluster = GetPartitionClusterCount(partNo) - f_cluster;
	if (nb_cluster > NB_CLUSTER_GROUP)
		nb_cluster = NB_CLUSTER_GROUP;

	/* Allocate memory */
	data   = (u8 *)calloc(SIZE_CLUSTER_DATA * NB_CLUSTER_GROUP, 1);
	header = (u8 *)calloc(SIZE_CLUSTER_HEADER * NB_CLUSTER_GROUP, 1);
	if (!data || !header)
		return false;

	// if we are replacing a full set of clusters then we don't
	// need to do any reading as we just need to overwrite the
	// blanked data


	// calculate number of clusters of data to write
	nClusters = ((nBytesToWrite -1)/ SIZE_CLUSTER_DATA)+1;

	if (nBytesToWrite!=(NB_CLUSTER_GROUP*SIZE_CLUSTER_DATA))
	{
		/* Read group clusters and headers */
		for (i = 0; i < nb_cluster; i++)
		{
			u8 *d_ptr = &data[SIZE_CLUSTER_DATA * i];
			u8 *h_ptr = &header[SIZE_CLUSTER_HEADER * i];
			
			/* Read cluster */
			if (ReadCluster(partNo, f_cluster + i, d_ptr, h_ptr))
			{
				free(data);
				free(header);
				return false;
			}
		}
	}
	else
	{
		// memory already cleared
	}

	// now overwrite the data in the correct location
	// be it from file data or from the memory location
	/* Write new cluster and H0 table */
	pos_header  = ((cluster - f_cluster) * SIZE_CLUSTER_HEADER);
	pos_cluster = ((cluster - f_cluster) * SIZE_CLUSTER_DATA);


	// we read from either memory or a file
	if (NULL!=fIn)
	{
		fread(&data[pos_cluster + nClusterOffset],1, nBytesToWrite, fIn); 
	}
	else
	{
		// data
		memcpy(&data[pos_cluster + nClusterOffset], in, nBytesToWrite);
	}

	// now for each cluster we need to...
	for(j = 0; j < nClusters; j++)
	{
		// clear the data for the table
		memset(h0, 0, SIZE_H0);

		/* Calculate new clusters H0 table */
		for (i = 0; i < SIZE_CLUSTER_DATA; i += 0x400)
		{
			u32 idx = (i / 0x400) * 20;
			
			/* Calculate SHA-1 hash */
			sha1(&data[pos_cluster + (j * SIZE_CLUSTER_DATA) + i], 0x400, &h0[idx]);
		}
		
		// save the H0 data
		memcpy(&header[pos_header + (j * SIZE_CLUSTER_HEADER)], h0, SIZE_H0);

		// now do the H1 data for the subgroup
		/* Calculate H1's */
		sha1(&header[pos_header + (j * SIZE_CLUSTER_HEADER)], SIZE_H0, h1);

		// now copy to all the sub cluster locations
		for (int k=0; k < NB_CLUSTER_SUBGROUP; k++)
		{
			// need to get the position of the first block we are changing
			// which is the start of the subgroup for the current cluster 
			u32 nSubGroup = ((cluster + j) % NB_CLUSTER_GROUP) / NB_CLUSTER_SUBGROUP;

			u32 pos = (SIZE_CLUSTER_HEADER * nSubGroup * NB_CLUSTER_SUBGROUP) + (0x14 * ((cluster +j)%NB_CLUSTER_SUBGROUP));

			memcpy(&header[pos + (k * SIZE_CLUSTER_HEADER) + OFFSET_H1], h1, 20);
		}

	}


	// now we need to calculate the H2's for all subgroups
	/* Calculate H2 */
	for (i = 0; i < NB_CLUSTER_SUBGROUP; i++)
	{
		u32 pos = (NB_CLUSTER_SUBGROUP * i) * SIZE_CLUSTER_HEADER;
		
		/* Cluster exists? */
		if ((pos / SIZE_CLUSTER_HEADER) > nb_cluster)
			break;
		
		/* Calculate SHA-1 hash */
		sha1(&header[pos + OFFSET_H1], SIZE_H1, &h2[20 * i]);
	}
	
	/* Write H2 table */
	for (i = 0; i < nb_cluster; i++)
	{
		/* Write H2 table */
		memcpy(&header[(SIZE_CLUSTER_HEADER * i) + OFFSET_H2], h2, SIZE_H2);
	}

	// update the H3 key table here
	/* Calculate SHA-1 hash */
	sha1(h2, SIZE_H2, &h3[group * 0x14]);


	// now encrypt and write
	
	/* Set title key */
	title_key = &(Image->Partitions[PartNo].TitleKey[0]);
	/* Encrypt headers */
	for (i = 0; i < nb_cluster; i++)
	{
		u8 *ptr = &header[SIZE_CLUSTER_HEADER * i];

		u8 phData[SIZE_CLUSTER_HEADER];

		/* Set IV key */
		memset(iv, 0, 16);

		/* Encrypt */
		aes_cbc_enc(ptr, (u8*) phData, SIZE_CLUSTER_HEADER, title_key, iv);
		memcpy(ptr, (u8*)phData, SIZE_CLUSTER_HEADER);
	}

	/* Encrypt clusters */
	for (i = 0; i < nb_cluster; i++)
	{
		u8 *d_ptr = &data[SIZE_CLUSTER_DATA * i];
		u8 *h_ptr = &header[SIZE_CLUSTER_HEADER * i];

		u8 phData[SIZE_CLUSTER_DATA];


		/* Set IV key */
		memcpy(iv, &h_ptr[OFFSET_CLUSTER_IV], 16);

		/* Encrypt */
		aes_cbc_enc(d_ptr, (u8*) phData, SIZE_CLUSTER_DATA, title_key, iv);
		memcpy(d_ptr, (u8*)phData, SIZE_CLUSTER_DATA);
	}

	/* Jump to first cluster in the group */
	offset = Image->Partitions[partNo].Offset + Image->Partitions[partNo].DataOffset + (u64)((u64)f_cluster * (u64)SIZE_CLUSTER);

	for (i = 0; i < nb_cluster; i++)
	{
		u8 *d_ptr = &data[SIZE_CLUSTER_DATA * i];
		u8 *h_ptr = &header[SIZE_CLUSTER_HEADER * i];

		if (true==DiscWriteDirect(offset, h_ptr, SIZE_CLUSTER_HEADER))
		{
			// written ok, add value to offset
			offset = offset + SIZE_CLUSTER_HEADER;

			if (true==DiscWriteDirect(offset, d_ptr, SIZE_CLUSTER_DATA))
			{
				offset = offset + SIZE_CLUSTER_DATA;
			}
			else
			{
				free(data);
				free(header);
				return false;

			}
		}
		else
		{
			// free memory and return error
			free(data);
			free(header);
			return false;
		}
	}


	// already calculated the H3 and H4 hashes - rely on surrounding code to
	// read and write those out

	/* Free memory */
	free(data);
	free(header);

	return true;
}


int Disc::GetPartitionClusterCount(int partNo)
{
	int nRetVal = 0;
	nRetVal = (int)(Image->Partitions[partNo].data_size / SIZE_CLUSTER);
	return nRetVal;
}

int Disc::ReadCluster(int partNo, int clusterNo, u8 * data, u8 * header)
{
	u8 buf[SIZE_CLUSTER];
	u8  iv[16];
	u8 * title_key;
	u64 offset;


	/* Jump to the specified cluster and copy it to memory */
	offset = Image->Partitions[partNo].Offset + Image->Partitions[partNo].DataOffset + (u64)((u64)clusterNo * (u64)SIZE_CLUSTER);
	
	// read the correct location block in
	Read(buf, SIZE_CLUSTER, offset);

	/* Set title key */
	title_key =  &(Image->Partitions[partNo].TitleKey[0]);

	/* Copy header if required*/
	if (header)
	{
		/* Set IV key to all 0's*/
		memset(iv, 0, sizeof(iv));

		/* Decrypt cluster header */
		aes_cbc_dec(buf, header, SIZE_CLUSTER_HEADER, title_key, iv);
	}

	/* Copy data if required */
	if (data)
	{
		/* Set IV key to correct location*/
		memcpy(iv, &buf[OFFSET_CLUSTER_IV], 16);

		/* Decrypt cluster data */
		aes_cbc_dec(&buf[0x400], data, SIZE_CLUSTER_DATA, title_key,  &iv[0]);

	}

	return 0;
}

void Disc::aes_cbc_enc(u8 *in, u8 *out, u32 len, u8 *key, u8 *iv)
{
	AES_KEY aes_key;

	/* Set encryption key */
	AES_set_encrypt_key(key, 128, &aes_key);

	/* Decrypt data */
	AES_cbc_encrypt(in, out, len, &aes_key, iv, AES_ENCRYPT);
}

void Disc::aes_cbc_dec(u8 *in, u8 *out, u32 len, u8 *key, u8 *iv)
{
	AES_KEY aes_key;

	/* Set decryption key */
	AES_set_decrypt_key(key, 128, &aes_key);

	/* Decrypt data */
	AES_cbc_encrypt(in, out, len, &aes_key, iv, AES_DECRYPT);
}

void Disc::sha1(u8 *data, u32 len, u8 *hash)
{
	SHA1(data, len, hash);
}

bool Disc::DiscWriteDirect(u64 nOffset, u8 *pData, unsigned int nSize)
{

	_int64 nSeek;

	// Simply seek to the right place
	nSeek = fseeko64(Image->File, nOffset + m_nDiscOffset, SEEK_SET);

    if (-1==nSeek)
	{
		_lastErr = "Unable to write to image. Seek error";
        return false;
    }

	//if (nSize!= _write(image->fp, pData, nSize))
	if (nSize!= fwrite(pData, 1, nSize, Image->File))
	{
		_lastErr = "Unable to write to image. Write error";
        return false;
	}
	return true;
}