#pragma once
#include <string>
using namespace std;
///<summary>
/// Contains utility functions
///</summary>
class Utils
{
public:
	Utils(void);
	~Utils(void);

	static string GetFileExtension(string inputFileName);
};
