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
		partition_folder * folder = d->Image->Partitions[1].Folders.Retrieve(0)->SubFolders.Retrieve(10);
		u64 id = folder->Files.Find("slowride.pak.ngc");
		PartitionFile * file = folder->Files.Retrieve(id);
		d->ReplaceFile("c:\\slowride.pak.ngc", file, true);

	}
	else
	{
		printf("false");
	}

	free(d);
	//d.Open("fdf");
	//Disc::Open("");
	
}

