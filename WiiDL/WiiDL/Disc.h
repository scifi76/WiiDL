#pragma once
// Disc class - represents a Wii disc image (ISO) and contains most of the methods for
// working with the image

#include <string>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

extern "C"
{

	class __declspec(dllexport) Disc
	{
		public:
			// public variables
			bool IsOpen;

			// Constructor and Destructor
			Disc(char* IsoFileName);
			~Disc(void);

			// public methods
			void Disc::Open(char* IsoFileName);
			const char* Disc::GetLastError();


			
		private:
			// private variables
			string _isoFileName;
			string _lastErr;
	};
}