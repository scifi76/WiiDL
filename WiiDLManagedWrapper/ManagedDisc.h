#pragma once
 
 
#include "Disc.h"
 
using namespace System;
 
namespace WiiDLManagedWrapper {
 
	public ref class ManagedPartHeader
	{
	public:
		ManagedPartHeader(part_header * header);
		~ManagedPartHeader();
		
		property char DiscId
		{
			char get();
		};

		property unsigned char IsGC
		{
			unsigned char get();
		};

		property unsigned char IsWii
		{
			unsigned char get();
		};

		property bool IsKoreanKey
		{
			bool get();
		};

		property char Gamecode[2]
		{
			char[2] get();
		};

		property char Region
		{
			char get();
		};

		property char Publisher[2]
		{
			char[2] get();
		};







	private:
		part_header * _header
	};


	public ref class ManagedImageFile
	{
	public:
		ManagedImageFile(image_file * image);
		~ManagedImageFile();

	property UInt64 DiscOffset
    {
		UInt64 get();
    };

	property UInt64 ImageSize
	{
		UInt64 get();
	};

	property bool IsReadOnly
	{
		bool get();
	}

	property UInt32 PartitionCount
	{
		UInt32 get();
	}

	property UInt64 FileCount
	{
		UInt64 get();
	}

	property UInt64 BytesCount
	{
		UInt64 get();
	}

	property unsigned char PrimaryCount
	{
		unsigned char get();
	}

	property unsigned char SecondaryCount
	{
		unsigned char get();
	}

	property unsigned char TertiaryCount
	{
		unsigned char get();
	}

	property unsigned char QuaternaryCount
	{
		unsigned char get();
	}

	property long long PrimaryTblOffset
	{
		long long get();
	}

	property long long SecondaryTblOffset
	{
		long long get();
	}

	property long long TertiaryTblOffset
	{
		long long get();
	}

	property long long QuaternaryTblOffset
	{
		long long get();
	}

	private:
		struct image_file * _image;
		ManagedPartHeader^ _header;
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

	ManagedImageFile^ Image;
    
 
  private:
    Disc * _disc;
    
  };


  
 
 
 
  
 
  
 
}
 