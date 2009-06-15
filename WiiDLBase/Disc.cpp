
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
				
				// read the header data
				if (Disc::Read(buffer, 0x440, _image, 0, false) == -1) 
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
			_lastErr = ex.what();
		}
	}

	return IsOpen;
}

///<summary>
/// Reads data from the iso into the buffer pointer
///<returns>The number of bytes read. Returns -1 if an error occurs</returns>
///</summary>
int Disc::Read (unsigned char * buffer, size_t size, struct image_file * image, u64 offset, bool markUsed)
{
        size_t byteCount;
		long long nSeek;

		nSeek = fseeko64(image->File, offset + image->DiscOffset, SEEK_SET);
		#if defined (__GNUC__) && defined(__unix__)
			// fseeko64 in unix always returns 0 on success so we need to use ftell to get the file position
			nSeek = ftello64(image->File);
		#endif

        if (nSeek==-1) 
		{
            return -1;
        }

		if (markUsed)
		{
			//MarkAsUsed(offset, size);
		}

        //bytes = _read(image->fp, ptr, size);

        //if (bytes != size)
                //AfxMessageBox("io_read");

        //return bytes;
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
			free(_image);
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

const char * Disc::GetLastError()
{
	// return the latest error message
	return _lastErr;
}


