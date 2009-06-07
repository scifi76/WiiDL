
#include "Disc.h"

///<summary>
/// Constructor. Creates a Disc object
///<param name="IsoFilename">The path of the Wii ISO file that the Disc object will access</param>
///</summary>
Disc::Disc(char* IsoFileName)
{
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
bool Disc::Open()
{
	if (!IsOpen) // dont bother if it's already open
	{
		try
		{
			// try to open the iso
			_fsIsoFile.open(_isoFileName, std::ios::in | std::ios::out | std::ios::binary);
			IsOpen = _fsIsoFile.is_open();

			// check if this is a devkitimage (.RVM)
			
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

const char* Disc::GetLastError()
{
	// return the latest error message
	return _lastErr;
}


