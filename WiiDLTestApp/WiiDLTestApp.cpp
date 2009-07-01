// WiiDLTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Disc.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//Open("Test");
	Disc* d = new Disc("X:\\Downloads\\Complete\\su-ssp\\su-ssp.iso");
	d->Load(true);
	if (d->IsLoaded)
	{
		printf("true");
		d->CloseFile();
		d->Load(false);
		d->CloseFile();
	}
	else
	{
		printf("false");
	}

	free(d);
	//d.Open("fdf");
	//Disc::Open("");
	
}

