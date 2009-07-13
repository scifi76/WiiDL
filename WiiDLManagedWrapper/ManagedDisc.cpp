
#include "stdafx.h"
#include "ManagedDisc.h"
using namespace System::Runtime::InteropServices;


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
		delete _disc;
	}

	String^ ManagedDisc::LastError::get()
	{
		return gcnew String(_disc->GetLastError().c_str());
	}

	bool ManagedDisc::IsLoaded::get()
    {
        return _disc->IsLoaded;
    }

	String^ ManagedDisc::ImageFileName::get()
	{
		return gcnew String(_disc->ImageFileName.c_str());
	}





	bool ManagedDisc::Load(bool readOnly)
	{
		bool retVal = _disc->Load(readOnly);
		Image = gcnew ManagedImageFile(_disc->Image);
		return retVal;
	}

	bool ManagedDisc::CloseFile()
	{
		return _disc->CloseFile();
	}

	UInt64 ManagedDisc::Read(unsigned char * buffer, UInt32 size, UInt64 offset, bool markUsed)
	{
		return _disc->Read(buffer, size, offset, markUsed);
	}

	UInt64 ManagedDisc::ReadFromPartition(unsigned char * buffer, UInt32 size, UInt32 partNo, UInt64 offset)
	{
		return _disc->ReadFromPartition(buffer, size, partNo, offset);
	}

	struct part_header * ManagedDisc::ParseImageHeader(u8 * inputData)
	{
		return _disc->ParseImageHeader(inputData);
	}

	void ManagedDisc::LoadKey(bool korean)
	{
		_disc->LoadKey(korean);
	}

	int ManagedDisc::MarkAsUsed(UInt64 nOffset, UInt64 nSize)
	{
		return _disc->MarkAsUsed(nOffset, nSize);
	}

	int ManagedDisc::MarkAsUsedCrypto(UInt64 partOffset, UInt64 offset, UInt64 size, bool isEncrypted)
	{
		return _disc->MarkAsUsedCrypto(partOffset, offset, size, isEncrypted);
	}

	int ManagedDisc::ParseImage()
	{
		return _disc->ParseImage();
	}
		
	int ManagedDisc::ParsePartitions()
	{
		return _disc->ParsePartitions();
	}
		
	tmd * ManagedDisc::TmdLoad(UInt32 partNo)
	{
		return _disc->TmdLoad(partNo);
	}


	

	// MANAGEDIMAGEFILE
	ManagedImageFile::ManagedImageFile(image_file * image)
	{
		_image = image;
		_header = gcnew ManagedPartHeader(_image->ImageHeader);
	}

	ManagedImageFile::~ManagedImageFile()
	{
		delete _image;
		delete _header;
	}

	UInt64 ManagedImageFile::DiscOffset::get()
	{
		return _image->DiscOffset;
	}

	UInt64 ManagedImageFile::ImageSize::get()
	{
		return _image->ImageSize;
	}

	bool ManagedImageFile::IsReadOnly::get()
	{
		return _image->IsReadOnly;
	}

	UInt32 ManagedImageFile::PartitionCount::get()
	{
		return _image->PartitionCount;
	}

	UInt64 ManagedImageFile::FileCount::get()
	{
		return _image->FileCount;
	}

	UInt64 ManagedImageFile::BytesCount::get()
	{
		return _image->BytesCount;
	}

	unsigned char ManagedImageFile::PrimaryCount::get()
	{
		return _image->PrimaryCount;
	}

	unsigned char ManagedImageFile::SecondaryCount::get()
	{
		return _image->SecondaryCount;
	}

	unsigned char ManagedImageFile::TertiaryCount::get()
	{
		return _image->TertiaryCount;
	}

	unsigned char ManagedImageFile::QuaternaryCount::get()
	{
		return _image->QuaternaryCount;
	}

	long long ManagedImageFile::PrimaryTblOffset::get()
	{
		return _image->PrimaryTblOffset;
	}

	long long ManagedImageFile::SecondaryTblOffset::get()
	{
		return _image->SecondaryTblOffset;
	}

	long long ManagedImageFile::TertiaryTblOffset::get()
	{
		return _image->TertiaryTblOffset;
	}

	long long ManagedImageFile::QuaternaryTblOffset::get()
	{
		return _image->QuaternaryTblOffset;
	}


	// MANAGEDPARTHEADER
	ManagedPartHeader::ManagedPartHeader(part_header * header)
	{
		_header = header;
	}

	ManagedPartHeader::~ManagedPartHeader()
	{
		delete _header;
	}
}
