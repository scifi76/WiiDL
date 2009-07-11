#pragma once


#include "Disc.h"

using namespace System;

namespace WiiDLManagedWrapper {

	public ref class ManagedImageFile
	{
	public:
		// constructor / destructor
		ManagedImageFile(image_file * image);
		~ManagedImageFile();

		// public methods
		UInt64 GetBytesCount();

	private:
		image_file * _image;
	};

	///<summary>
	/// This is a managed (.NET) wrapper class around the unmanaged Disc object
	///</summary>
	public ref class ManagedDisc
	{
	public:
		
		// constructor / destructor
		ManagedDisc(String^ IsoFileName);
		~ManagedDisc();

		// properties (C++ properties are only supported in CLI/C++ but since this class is only to be used
		// by managed code, it doesn't matter)
		property String^ LastError
		{
			String^ get();
		};

		property bool IsLoaded
		{
			bool get();
		};

		property String^ ImageFileName
		{
			String^ get();
		};
		
		property ManagedImageFile Image
		{
			ManagedImageFile get();
		};

		// public methods
		bool Load(bool readOnly);
		bool CloseFile();
		UInt64 Read (unsigned char * buffer, UInt32 size, UInt64 offset, bool markUsed);
		UInt64 ReadFromPartition(unsigned char * buffer, UInt32 size, UInt32 partNo, UInt64 offset);
		struct part_header * ParseImageHeader(u8 * inputData);
		void LoadKey(bool korean);
		int MarkAsUsed(UInt64 nOffset, UInt64 nSize);
		int MarkAsUsedCrypto(UInt64 partOffset, UInt64 offset, UInt64 size, bool isEncrypted);
		int ParseImage();
		int ParsePartitions();
		tmd * TmdLoad(UInt32 partNo);
		
		

	private:
		Disc * _disc;
		
	};



	

	

}
