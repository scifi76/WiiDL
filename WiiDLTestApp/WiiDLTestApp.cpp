// WiiDLTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Disc.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//Open("Test");
	Disc* d = new Disc("X:\\Downloads\\Complete\\su-hhrr\\su-hhrr.iso");
	d->Open(true);
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

