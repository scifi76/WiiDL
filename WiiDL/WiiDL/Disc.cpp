#include "Disc.h"

extern "C"
{
	// Opens an iso image file and returns the file handle
	int Disc::Open(char* IsoFileName)
	{
		// try to open the iso
		int fHandle;
		errno_t fErr;
		
		// the old wii scrubber used the deprecated _open method. Not sure why, but apparently this is better...
		fErr = _sopen_s(&fHandle, IsoFileName, _O_BINARY|_O_RDWR, _SH_DENYNO, _S_IREAD|_S_IWRITE);
			
		if (fHandle >0)
		{
			// success
			return fHandle;
		}
		else
		{
			// failed. set _lastErr
			switch (fErr) 
			{
				case EACCES: 
					//_lastErr = "Unable to open file: Permission denied, Read only file or File in use";
					return -1;
				case EMFILE:  
					//_lastErr = "Unable to open file: No file handle";
					return -2;
				case ENOENT:  
					//_lastErr = "Unable to open file: File or path not found";
					return -3;
				default:      
					//_lastErr = "Unable to open file: Unknown error";
					return -4;
			}
			
		}
	}


}
