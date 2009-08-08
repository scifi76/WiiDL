#pragma once
 
 
#include "Disc.h"
 
using namespace System;
 
namespace WiiDLManagedWrapper {
 
	public ref class MFile
	{
	public:
		MFile(partition_file * file);
		~MFile();
		
		property UInt64 Offset
		{
			UInt64 get();
		};

		property UInt64 Size
		{
			UInt64 get();
		};

		property String^ FileName
		{
			String^ get();
		};

		
	private:
		partition_file * _file;
	};

	public ref class MFolder
	{
	public:
		MFolder(partition_folder * folder);
		~MFolder();
		
		property String^ FolderName
		{
			String^ get();
		};

		System::Collections::Generic::List<MFile^>^ Files;

	private:
		partition_folder * _folder;
	};

	public ref class MPartHeader
	{
	public:
		MPartHeader(part_header * header);
		~MPartHeader();
		
		property char DiscId
		{
			char get();
		};

		property bool IsGC
		{
			bool get();
		};

		property bool IsWii
		{
			bool get();
		};

		property bool IsKoreanKey
		{
			bool get();
		};

		property String^ Gamecode
		{
			String^ get();
		};

		property char Region
		{
			char get();
		};

		property String^ Publisher
		{
			String^ get();
		};

		property bool HasMagic
		{
			bool get();
		};

		property String^ GameTitle
		{
			String^ get();
		};

		property UInt64 DolOffset
		{
			UInt64 get();
		};

		property UInt64 DolSize
		{
			UInt64 get();
		};

		property UInt64 FstOffset
		{
			UInt64 get();
		};

		property UInt64 FstSize
		{
			UInt64 get();
		};


	private:
		part_header * _header;
	};

	public ref class MPartition
	{
	public:
		MPartition(partition * part);
		~MPartition();

		MPartHeader^ Header;

		property UInt64 Offset
		{
			UInt64 get();
		};

		property UInt64 AppldrSize
		{
			UInt64 get();
		};

		property bool IsEncrypted
		{
			bool get();
		};

		property bool IsValid
		{
			bool get();
		};

		property UInt64 TmdOffset
		{
			UInt64 get();
		};

		property UInt64 TmdSize
		{
			UInt64 get();
		};

		property UInt64 H3Offset
		{
			UInt64 get();
		};

		property String^ TitleIdString
		{
			String^ get();
		};

		property String^ Type
		{
			String^ get();
		};

		property String^ ChannelId
		{
			String^ get();
		}

		property UInt64 DataOffset
		{
			UInt64 get();
		};

		property UInt64 DataSize
		{
			UInt64 get();
		};

		property UInt64 CertOffset
		{
			UInt64 get();
		};

		property UInt64 CertSize
		{
			UInt64 get();
		};

		System::Collections::Generic::List<MFolder^>^ Folders;

	private:
		partition * _partition;
	};

	


	public ref class MImageFile
	{
	public:
		MImageFile(image_file * image);
		~MImageFile();

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

	property UInt64 PrimaryTblOffset
	{
		UInt64 get();
	}

	property UInt64 SecondaryTblOffset
	{
		UInt64 get();
	}

	property UInt64 TertiaryTblOffset
	{
		UInt64 get();
	}

	property UInt64 QuaternaryTblOffset
	{
		UInt64 get();
	}

	MPartHeader^ Header;
	System::Collections::Generic::List<MPartition^>^ Partitions;

	private:
		struct image_file * _image;
		
		
	};

  ///<summary>
  /// This is a managed (.NET) wrapper class around the unmanaged Disc object
  ///</summary>
  public ref class MDisc
  {
  public:
 
    // constructor / destructor
    MDisc(String^ IsoFileName);
    ~MDisc();
 
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

	MImageFile^ Image;
    
 
  private:
    Disc * _disc;
    
  };


  
 
 
 
  
 
  
 
}
 