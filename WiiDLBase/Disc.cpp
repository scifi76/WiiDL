
#include "Disc.h"
#include "Utils.h"


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
			_fIsoFile = fopen(_isoFileName.c_str(), mode.c_str());

			//check the file opened ok
			if (_fIsoFile != NULL)
			{
				IsOpen = true;

				// check if this is a devkitimage (.RVM)
				_isoExtension = Utils::StringToUpper(Utils::GetFileExtension(_isoFileName));
				
				if (_isoExtension == "RVM")
				{
					// devkit images have an additional 32k header. We need to skip it
					_discOffset = 0x8000;
				}
				else
				{
					// no devkit image, so no offset
					_discOffset = 0;
				}

				// get the size of the file not including the devkit header
				_imageSize = fseeko64(_fIsoFile, 0L, SEEK_END);
				#if defined (__GNUC__) && defined(__unix__)
					// fseeko64 in unix returns 0 on success so we need to use ftell to get the file position
					_imageSize = ftello64(_fIsoFile);
				#endif
				_imageSize -= _discOffset;

			}
			else
			{
				throw std::ios::failure("Unable to open file. Check file exists and is writeable");
			}
		}
		catch (std::ios::failure ex)
		{
			// failed...
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
			fclose(_fIsoFile);
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


