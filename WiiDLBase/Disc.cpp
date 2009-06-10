
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
			// try to open the iso
			if (readOnly)
			{
				// open for read
				_fsIsoFile.open(_isoFileName.c_str(), std::ios::in | std::ios::binary);
			}
			else
			{
				// open for read and write
				_fsIsoFile.open(_isoFileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);
			}

			IsOpen = _fsIsoFile.is_open();

			//check the file opened ok
			if (IsOpen)
			{
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
				_imageSize = _fsIsoFile.tellg();
				_fsIsoFile.seekg(0,ios::end);
				_imageSize = _fsIsoFile.tellg();
				_fsIsoFile.seekg(0, ios::beg);
				_imageSize = _fsIsoFile.tellg();



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
			_fsIsoFile.close(); // close the ISO
			IsOpen = _fsIsoFile.is_open();
		}
		catch (std::ios::failure ex)
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


