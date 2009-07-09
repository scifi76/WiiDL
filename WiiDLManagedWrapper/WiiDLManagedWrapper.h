#pragma once


#include "Disc.h"

using namespace System;

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

		

		// public methods
		bool Load(bool readOnly);
		bool CloseFile();
		String^ GetLastError();
		UInt64 Read (unsigned char * buffer, UInt32 size, UInt64 offset, bool markUsed);
		UInt64 ReadFromPartition(unsigned char * buffer, UInt32 size, UInt32 partNo, UInt64 offset);
		struct part_header * ParseImageHeader(u8 * inputData);
		void LoadKey(bool korean);
		int MarkAsUsed(UInt64 nOffset, UInt64 nSize);
		int MarkAsUsedCrypto(UInt64 partOffset, UInt64 offset, UInt64 size, bool isEncrypted);
		int ParseImage();
		int ParsePartitions();
		tmd * TmdLoad(UInt32 partNo);
		bool IsLoaded();
		struct image_file * GetImage();

	private:
		Disc * _disc;

	};

}
