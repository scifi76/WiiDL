
#include "stdafx.h"
#include "ManagedDisc.h"
using namespace System::Runtime::InteropServices;


namespace WiiDLManagedWrapper 
{
	MDisc::MDisc(String^ IsoFileName)
	{
		char* fn = (char*)(void*)Marshal::StringToHGlobalAnsi(IsoFileName);
		_disc = new Disc(fn);
	}

	MDisc::~MDisc()
	{
		_disc->~Disc();
		delete _disc;
	}

	String^ MDisc::LastError::get()
	{
		return gcnew String(_disc->GetLastError().c_str());
	}

	bool MDisc::IsLoaded::get()
    {
        return _disc->IsLoaded;
    }

	String^ MDisc::ImageFileName::get()
	{
		return gcnew String(_disc->ImageFileName.c_str());
	}

	bool MDisc::Load(bool readOnly)
	{
		bool retVal = _disc->Load(readOnly);
		Image = gcnew MImageFile(_disc->Image);
		return retVal;
	}

	bool MDisc::CloseFile()
	{
		return _disc->CloseFile();
	}

	UInt64 MDisc::Read(unsigned char * buffer, UInt32 size, UInt64 offset, bool markUsed)
	{
		return _disc->Read(buffer, size, offset, markUsed);
	}

	UInt64 MDisc::ReadFromPartition(unsigned char * buffer, UInt32 size, UInt32 partNo, UInt64 offset)
	{
		return _disc->ReadFromPartition(buffer, size, partNo, offset);
	}

	struct part_header * MDisc::ParseImageHeader(u8 * inputData)
	{
		return _disc->ParseImageHeader(inputData);
	}

	void MDisc::LoadKey(bool korean)
	{
		_disc->LoadKey(korean);
	}

	int MDisc::MarkAsUsed(UInt64 nOffset, UInt64 nSize)
	{
		return _disc->MarkAsUsed(nOffset, nSize);
	}

	int MDisc::MarkAsUsedCrypto(UInt64 partOffset, UInt64 offset, UInt64 size, bool isEncrypted)
	{
		return _disc->MarkAsUsedCrypto(partOffset, offset, size, isEncrypted);
	}

	int MDisc::ParseImage()
	{
		return _disc->ParseImage();
	}
		
	int MDisc::ParsePartitions()
	{
		return _disc->ParsePartitions();
	}
		
	tmd * MDisc::TmdLoad(UInt32 partNo)
	{
		return _disc->TmdLoad(partNo);
	}

	bool MDisc::ExtractFile(System::String^ destFilename, MFile^ file, bool decrypt)
	{
		// Marshal the managed string to unmanaged memory.
		char* dest = (char*) Marshal::StringToHGlobalAnsi(destFilename).ToPointer();
		return _disc->ExtractFile(dest, file->_file, decrypt);
	}

	bool MDisc::ReplaceFile(System::String^ inputFilename, MFile^ file, bool encrypt)
	{
		// Marshal the managed string to unmanaged memory.
		char* inputFile = (char*) Marshal::StringToHGlobalAnsi(inputFilename).ToPointer();
		return _disc->ReplaceFile(inputFile, file->_file, encrypt);
	}

	

	// MANAGEDIMAGEFILE
	MImageFile::MImageFile(image_file * image)
	{
		_image = image;
		Header = gcnew MPartHeader(_image->ImageHeader);

		// get partitions
		Partitions = gcnew System::Collections::Generic::List<MPartition^>();
		u32 i;
		for (i = 0; i < _image->PartitionCount; ++i)
		{

			Partitions->Add(gcnew MPartition(&_image->Partitions[i]));
		}
	}

	MImageFile::~MImageFile()
	{
		delete _image;
		delete Header;
	}

	UInt64 MImageFile::DiscOffset::get()
	{
		return _image->DiscOffset;
	}

	UInt64 MImageFile::ImageSize::get()
	{
		return _image->ImageSize;
	}

	bool MImageFile::IsReadOnly::get()
	{
		return _image->IsReadOnly;
	}

	UInt32 MImageFile::PartitionCount::get()
	{
		return _image->PartitionCount;
	}

	UInt64 MImageFile::FileCount::get()
	{
		return _image->FileCount;
	}

	UInt64 MImageFile::BytesCount::get()
	{
		return _image->BytesCount;
	}

	unsigned char MImageFile::PrimaryCount::get()
	{
		return _image->PrimaryCount;
	}

	unsigned char MImageFile::SecondaryCount::get()
	{
		return _image->SecondaryCount;
	}

	unsigned char MImageFile::TertiaryCount::get()
	{
		return _image->TertiaryCount;
	}

	unsigned char MImageFile::QuaternaryCount::get()
	{
		return _image->QuaternaryCount;
	}

	UInt64 MImageFile::PrimaryTblOffset::get()
	{
		return _image->PrimaryTblOffset;
	}

	UInt64 MImageFile::SecondaryTblOffset::get()
	{
		return _image->SecondaryTblOffset;
	}

	UInt64 MImageFile::TertiaryTblOffset::get()
	{
		return _image->TertiaryTblOffset;
	}

	UInt64 MImageFile::QuaternaryTblOffset::get()
	{
		return _image->QuaternaryTblOffset;
	}


	// MPARTHEADER
	MPartHeader::MPartHeader(part_header * header)
	{
		_header = header;
	}

	MPartHeader::~MPartHeader()
	{
		delete _header;
	}

	char MPartHeader::DiscId::get()
	{
		return _header->DiscId;
	}

	bool MPartHeader::IsGC::get()
	{
		if (_header->IsGC == 1)
			return true;
		else
			return false;
	}

	bool MPartHeader::IsWii::get()
	{
		if (_header->IsWii == 1)
			return true;
		else
			return false;
	}

	bool MPartHeader::IsKoreanKey::get()
	{
		return _header->IsKoreanKey;
	}

	String^ MPartHeader::Gamecode::get()
	{
		return gcnew String(_header->Gamecode,0, 2);
	}

	char MPartHeader::Region::get()
	{
		return _header->Region;
	}

	String^ MPartHeader::Publisher::get()
	{
		return gcnew String(_header->Publisher, 0, 2);
	}

	bool MPartHeader::HasMagic::get()
	{
		return _header->HasMagic;
	}
	
	String^ MPartHeader::GameTitle::get()
	{
		return gcnew String(_header->GameTitle);
	}

	UInt64 MPartHeader::DolOffset::get()
	{
		return _header->DolOffset;
	}

	UInt64 MPartHeader::DolSize::get()
	{
		return _header->DolSize;
	}

	UInt64 MPartHeader::FstOffset::get()
	{
		return _header->FstOffset;
	}

	UInt64 MPartHeader::FstSize::get()
	{
		return _header->FstSize;
	}




	//MPartition
	MPartition::MPartition(partition * part)
	{
		_partition = part;
		Header = gcnew MPartHeader(part->Header);

		Folders = gcnew System::Collections::Generic::List<MFolder^>();
		u32 i;

		for (i = 0; i < _partition->Folders.Count(); ++i)
		{
			Folders->Add(gcnew MFolder(_partition->Folders.Retrieve(i)));
		}
	}

	MPartition::~MPartition()
	{
		//delete _partition;
		delete Header;
	}

	UInt64 MPartition::Offset::get()
	{
		return _partition->Offset;
	}

	UInt64 MPartition::AppldrSize::get()
	{
		return _partition->AppldrSize;
	}

	bool MPartition::IsEncrypted::get()
	{
		return _partition->IsEncrypted;
	}

	bool MPartition::IsValid::get()
	{
		return _partition->IsValid;
	}

	UInt64 MPartition::TmdOffset::get()
	{
		return _partition->TmdOffset;
	}

	UInt64 MPartition::TmdSize::get()
	{
		return _partition->TmdSize;
	}

	UInt64 MPartition::H3Offset::get()
	{
		return _partition->H3Offset;
	}

	String^ MPartition::TitleIdString::get()
	{
		return gcnew String(_partition->TitleIdString);
	}

	String^ MPartition::Type::get()
	{
		String^ retVal = "";
		switch (_partition->Type)
		{
		case PART_UNKNOWN:
			retVal = "UNKNOWN";
			break;
		case PART_DATA:
			retVal = "DATA";
			break;
		case PART_UPDATE:
			retVal = "UPDATE";
			break;
		case PART_INSTALLER:
			retVal = "INSTALLER";
			break;
		case PART_VC:
			retVal = "VC";
			break;
		}

		 return retVal;
	}

	String^ MPartition::ChannelId::get()
	{
		return gcnew String(_partition->ChannelId);
	}

	UInt64 MPartition::DataOffset::get()
	{
		return _partition->DataOffset;
	}

	UInt64 MPartition::DataSize::get()
	{
		return _partition->DataSize;
	}

	UInt64 MPartition::CertOffset::get()
	{
		return _partition->CertOffset;
	}

	UInt64 MPartition::CertSize::get()
	{
		return _partition->CertSize;
	}





	// MFILE
	MFile::MFile(PartitionFile * file)
	{
		_file = file;
	}

	MFile::~MFile()
	{
		delete _file;
	}

	UInt64 MFile::Offset::get()
	{
		return _file->Offset;
	}

	UInt64 MFile::Size::get()
	{
		return _file->Size;
	}

	UInt32 MFile::PartNo::get()
	{
		return _file->PartNo;
	}

	int MFile::FstRef::get()
	{
		return _file->FstRef;
	}

	String^ MFile::FileName::get()
	{
		return gcnew String(_file->FileName);
	}



	// MFOLDER
	MFolder::MFolder(partition_folder * folder)
	{
		_folder = folder;
		
		Files = gcnew System::Collections::Generic::List<MFile^>();
		
		u32 i;

		for (i = 0; i < folder->Files.Count(); ++i)
		{
			Files->Add(gcnew MFile(folder->Files.Retrieve(i)));
		}
		
		SubFolders = gcnew System::Collections::Generic::List<MFolder^>();

		for (i = 0; i < folder->SubFolders.Count(); ++i)
		{
			u32 x;
			MFolder^ curFolder = gcnew MFolder(folder->SubFolders.Retrieve(i));	
			SubFolders->Add(curFolder);
		}
	}

	MFolder::~MFolder()
	{
		delete _folder;
	}

	String^ MFolder::FolderName::get()
	{
		return gcnew String(_folder->FolderName);
	}

}
