#pragma once
// Disc class - represents a Wii disc image (ISO) and contains most of the methods for
// working with the image



// This ifdef block allows the header to export the DLL's methods when compiled
// from an app that defines DISC_EXPORTS (eg the Base). If DISC_EXPORTS is not
// defined then the methods will be imported instead. Anything defined with 
// DISC_API will be made available for export/import. Clever stuff.
#ifdef DISC_EXPORTS
#define DISC_API __declspec(dllexport)
#else
#define DISC_API __declspec(dllimport)
#pragma message("automatic link to Test.lib")   // <== add this line
#pragma comment(lib, "WiiDLBase.lib") 
#endif


class DISC_API Disc
{
	public:
		// public variables
		
		// public methods
		int Disc::Open(char* IsoFileName);
			

			
	private:
		// private variables
};
