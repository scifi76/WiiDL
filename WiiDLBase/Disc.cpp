
#include "Disc.h"
#include "Utils.h"
#include <string.h>



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
				struct part_header *header;
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
				
				//ParseImageHeader


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

const char * Disc::GetLastError()
{
	// return the latest error message
	return _lastErr;
}


