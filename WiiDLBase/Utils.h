#pragma once
#include <string>;
///<summary>
/// Contains utility functions
///</summary>
class Utils
{
public:
	Utils(void);
	~Utils(void);

	static char* GetFileExtension(char* inputFileName);
	static char* AppendCharToCharArray(const char *charArray, const char appendChar);
};
