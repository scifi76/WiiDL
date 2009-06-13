#pragma once
// Disc class - represents a Wii disc image (ISO) and contains most of the methods for
// working with the image

#include <fstream>
#include <string>
#include "Structs.h"



#define DISC_EXPORTS 

// This ifdef block allows the header to export the DLL's methods when compiled
// from an app that defines DISC_EXPORTS (eg the Base). If DISC_EXPORTS is not
// defined then the methods will be imported instead. Anything defined with 
// DISC_API will be made available for export/import. Clever stuff.
#if defined (__GNUC__) && defined(__unix__)
	#define DISC_API __attribute__ ((__visibility__("default")))
#elif defined (_MSC_VER)
	#include <io.h>
	#define fseeko64(fm, pos, type) _lseeki64(_fileno(fm), (pos), (type))
	
	#ifdef DISC_EXPORTS
		#define DISC_API __declspec(dllexport)
		
	#else
		#define DISC_API __declspec(dllimport)
		#pragma message("Linking to WiiBaseDL.lib...")
		#pragma comment(lib, "WiiDLBase.lib") 
	#endif
#endif




using namespace std;

///<summary>
/// Represents a Wii disc image (ISO) and contains most of the methods for working with the image
///</summary>
class DISC_API Disc
{
	public:
		// Constructor / Destructor
		Disc(char * IsoFileName);
		~Disc(void);

		// public variables
		bool IsOpen;

		// public methods
		bool Open(bool readOnly);
		bool Close();
		const char * GetLastError();
		
	private:
		// private variables
		string _isoFileName; // the name of the isofile
		string _isoExtension;
		FILE * _fIsoFile; // a stream object for accessing the file
		const char * _lastErr; // used to store the last error message that occured
		long long _discOffset; // the offset within the ISO file where the disc's data starts
		long long _imageSize; // the size of the disc image
		struct image_file * _image; // stores image file info. It will be populated when calling Open()
		
};
