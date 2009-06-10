#pragma once


#include "Disc.h"
using namespace System;
using namespace System::Runtime::InteropServices;




namespace WiiDLManagedWrapper {
	///<summary>
	/// This is a managed (.NET) wrapper class around the unmanaged Disc object
	///</summary>
	public ref class ManagedDisc
	{
	public:
		// constructor / destructor
		ManagedDisc(String^ IsoFileName);
		~ManagedDisc();

	private:
		Disc * _disc;

	};
	
}
