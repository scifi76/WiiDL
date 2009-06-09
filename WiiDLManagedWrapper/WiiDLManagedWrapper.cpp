// This is the main DLL file.

#include "stdafx.h"

#include "WiiDLManagedWrapper.h"

namespace WiiDLManagedWrapper 
{
	ManagedDisc::ManagedDisc()
	{
		_disc = new Disc("C:\test.iso.RVM");
	}

}
