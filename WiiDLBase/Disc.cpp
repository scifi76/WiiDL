#define DISC_EXPORTS 
#include "Disc.h"

// Constructor
Disc::Disc(char* IsoFileName)
{
	_isoFileName = IsoFileName;
}

// Opens an iso image file and returns the file handle
bool Disc::Open()
{
	// try to open the iso
	_fsIsoFile.open(_isoFileName, std::ios::in | std::ios::out | std::ios::binary);
	
	IsOpen = true;
	return true;;
}

void Disc::Close()
{
	// close the file;
	if (IsOpen)
	{
		_fsIsoFile.close();
		IsOpen = false;
	}
}


