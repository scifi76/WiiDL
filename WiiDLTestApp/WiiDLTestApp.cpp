// WiiDLTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Disc.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//Open("Test");
	Disc* d = new Disc("C:\\Test.iso");
	d->Open(false);
	if (d->IsOpen)
	{
		printf("true");
		d->Close();
	}
	else
	{
		printf("false");
	}

	
	//d.Open("fdf");
	//Disc::Open("");
	
}

