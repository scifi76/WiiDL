
#include "stdafx.h"
#include "WiiDLManagedWrapper.h"
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
	}

	bool ManagedDisc::Load(bool readOnly)
	{
		return _disc->Load(readOnly);	
	}

	bool ManagedDisc::CloseFile()
	{
		return _disc->CloseFile();
	}

	String^ ManagedDisc::GetLastError()
	{
		return gcnew String(_disc->GetLastError().c_str());
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

	bool ManagedDisc::IsLoaded()
    {
        return _disc->IsLoaded;
    }

	struct image_file * ManagedDisc::GetImage()
	{
		return _disc->Image;
	}

}
