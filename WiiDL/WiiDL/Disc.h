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
	class Disc
	{
		public:
			// public variables
			

			// public methods
			int Disc::Open(char* IsoFileName);
			

			
		private:
			// private variables
	};
}