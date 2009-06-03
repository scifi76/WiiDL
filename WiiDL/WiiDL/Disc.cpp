#include "Disc.h"


	// Constructor
	Disc::Disc(char* IsoFileName)
	{
		_isoFileName = IsoFileName;
		_lastErr = "";
	}

	// Destructor
	Disc::~Disc(void)
	{
	}

	void Disc::Open(char* IsoFileName)
	{
		_isoFileName = "";
		_isoFileName.assign(IsoFileName);

		// try to open the iso
		int fHandle;
		errno_t fErr;
		
		// the old wii scrubber used the deprecated _open method. Not sure why, but apparently this is better...
		fErr = _sopen_s(&fHandle, _isoFileName.c_str(), _O_BINARY|_O_RDWR, _SH_DENYNO, _S_IREAD|_S_IWRITE);
			
		if (fHandle >0)
		{
			// success
			IsOpen = true;
		}
		else
		{
			// failed. set _lastErr
			switch (fErr) 
			{
				case EACCES: 
					_lastErr = "Unable to open file: Permission denied, Read only file or File in use";
				case EMFILE:  
					_lastErr = "Unable to open file: No file handle";
				case ENOENT:  
					_lastErr = "Unable to open file: File or path not found";
				default:      
					_lastErr = "Unable to open file: Unknown error";
			}
			IsOpen = false;
		}
	}

	const char* Disc::GetLastError()
	{
		return _lastErr.c_str();
	}


