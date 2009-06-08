#include "Utils.h"

Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}

///<summary>
/// Returns the extension of the filename passed in
///<returns>The 3 character extension of the input file</returns>
///</summary>
char* Utils::GetFileExtension(char *inputFileName)
{
	char* retVal;
	int i;

	// loop until null is found
	for(i = 0; inputFileName[i]; i++)
	{
		retVal = Utils::AppendCharToCharArray(retVal, inputFileName[i]);
	}
	return retVal;
}

///<summary>
/// Appends a char to a char array
///<returns>A char array with the new char appended to it</returns>
///</summary>
char* Utils::AppendCharToCharArray(const char *charArray, const char appendChar)
{
	std::size_t len = std::strlen ( charArray );
	char *retVal = new char[len + 2];

	std::strcpy (retVal, charArray );
	retVal[len] = appendChar;
	retVal[len + 1] = '\0';

	return retVal;
}