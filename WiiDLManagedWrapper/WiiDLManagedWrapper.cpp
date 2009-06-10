// This is the main DLL file.

#include "stdafx.h"

#include "WiiDLManagedWrapper.h"



namespace WiiDLManagedWrapper 
{
	ManagedDisc::ManagedDisc(String^ IsoFileName)
	{
		char* fn = (char*)(void*)Marshal::StringToHGlobalAnsi(IsoFileName);
		_disc = new Disc(fn);
	}

	ManagedDisc::~ManagedDisc()
	{
		_disc->~Disc();
	}

}
