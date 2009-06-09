#pragma once
#include "Disc.h";

using namespace System;

namespace WiiDLManagedWrapper {
	///<summary>
	/// This is a managed (.NET) wrapper class around the unmanaged Disc object
	///</summary>
	public ref class ManagedDisc
	{
	public:
		// constructor / destructor
		ManagedDisc();
		~ManagedDisc();

	private:
		Disc * _disc;

	};
}
