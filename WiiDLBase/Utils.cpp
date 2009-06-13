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
string Utils::GetFileExtension(string inputFileName)
{
	std::string tempStr = inputFileName;
	std::string retVal = tempStr.substr(tempStr.find_last_of(".") + 1);
	return retVal;
}

///<summary>
/// Converts the string in inputString to uppercase
///<returns>An uppercase'd inputString</returns>
///</summary>
string Utils::StringToUpper(string inputString)
{
	for (unsigned int i=0; i<inputString.length(); ++i)
	{
		inputString[i]=toupper(inputString[i]);
	}
	return inputString;
}

