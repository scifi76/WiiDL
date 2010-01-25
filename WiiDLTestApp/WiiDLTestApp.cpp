// WiiDLTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Disc.h"

int _tmain(int argc, _TCHAR* argv[])
{

	//Open("Test");
	Disc* d = new Disc("C:\\Documents and Settings\\Tony\\Desktop\\gh3.iso");
	d->Load(true);
	if (d->IsLoaded)
	{
		printf("true");
		partition_folder * folder = d->Image->Partitions[0].Folders.Retrieve(0)->SubFolders.Retrieve(0);
		PartitionFile * file = folder->Files.Retrieve(5);
		d->ReplaceFile("c:\\vis.ini", file, true);

	}
	else
	{
		printf("false");
	}

	free(d);
	//d.Open("fdf");
	//Disc::Open("");
	
}

