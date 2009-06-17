
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
	IsOpen = false;
	_isoFileName = IsoFileName;
}

///<summary>
/// Destructor. Disposes of the Disc object
///</summary>
Disc::~Disc()
{
	// Close the ISO if it's open
	if (IsOpen)
	{
		Close();
	}
}

///<summary>
/// Opens the Wii ISO
///<param name="readOnly">Whether or not the ISO should be opened in read only mode. Opening in read only mode will prevent the image file from being locked to other applications but any write actions to the image will fail</param>
///<returns>True if the ISO was successfully opened. Otherwise false</returns>
///</summary>
bool Disc::Open(bool readOnly)
{
	if (!IsOpen) // dont bother if it's already open
	{
		try
		{
			
			string isoExtension;
			FILE * fIsoFile; // a stream object for accessing the file
			long long discOffset; // the offset within the ISO file where the disc's data starts
			long long imageSize; // the size of the disc image

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
				// file opened ok
				IsOpen = true;

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
				
				// add the read only flag to the struct
				_image->readOnly = readOnly;

				// read the header data
				if (Disc::Read(buffer, 0x440, 0, false) == -1) 
					throw std::ios::failure("Unable to read header from image file");
				
				// header data read ok if we got here so allocate some memory to store it in
				header = (struct part_header *) (malloc (sizeof (struct part_header)));

				if (!header) // memory allocation failed
					throw std::ios::failure("Unable to allocate memory for image header struct");
				
				header = ParseImageHeader(buffer);

				if (!header->HasMagic) // no magic word
					throw std::ios::failure("Image has bad magic word");

				if (header->IsWii)
				{
					// TODO: Check for key.bin file and use that if exists
					LoadKey(header->KoreanKey);
				}
				else
				{
					// the old wii scrubber simply doesn't load any key for gamecube images
				}



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
			Close();
			free(_image);
			_lastErr = ex.what();
		}
	}

	return IsOpen;
}

///<summary>
/// Closes the Wii ISO
///<returns>True if the ISO was successfully closed. Otherwise false</returns>
///</summary>
bool Disc::Close()
{
	// close the file;
	if (IsOpen) // dont bother if it's already closed
	{
		try
		{
			fclose(_image->File);
			//free(_image); -- we dont want to free the image. The user should still be able to close the file but still work with some of the loaded settings
			IsOpen = false;
		}
		catch (std::ios::failure ex) // i dont think this will ever catch anything now fstream class isnt being used. Wont harm to leave it here anyway
		{
			// failed...
			_lastErr = ex.what();
		}
	}
	
	return !IsOpen;
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
	if (Disc::IsOpen) // can't read if the disc isn't open
	{
        size_t byteCount;
		long long nSeek;
		
		// seek to the correct offset
		nSeek = fseeko64(_image->File, offset + _image->DiscOffset, SEEK_SET);
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

		byteCount = fread(buffer, 1, size, _image->File);
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
		_lastErr = "Cannot read data when image is closed. Please call Open() first";
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
		if (_image->pFreeTable[nStartValue / (u64)(0x8000)] != 1)
		{
			_image->pFreeTable[nStartValue / (u64)(0x8000)] = 1;
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
		header->KoreanKey = (inputData[0x1f1]==1);
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
	
	AES_set_decrypt_key (LoadedKey, 128, &_image->CommonKey);
}

const char * Disc::GetLastError()
{
	// return the latest error message
	return _lastErr;
}


