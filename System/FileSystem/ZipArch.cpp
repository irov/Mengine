#include "ZipArch.h"
#include <assert.h>

ZipArchive::ZipArchive(const std::string& _filename, int _prior) 
: mUzFile(0),
mPriority(_prior)
{
	assert( _prior > 0 );
	if(!LoadZip(_filename, _prior))
	{
		assert(!"error in FileSystem::ZipArchive()");
	}
} 

ZipArchive::~ZipArchive()
{
	UnloadZip();
}

bool ZipArchive::UnloadZip()
{
	unzCloseCurrentFile(mUzFile);
	int nRet = unzClose(mUzFile);
	mUzFile = NULL;
	return (nRet == UNZ_OK);
}

bool ZipArchive::LoadZip(const std::string& _filename, int _prior) 
{
	assert( !_filename.empty() );

	mZipName = _filename;
	mPriority = _prior;
	UnloadZip();
	mUzFile = unzOpen( _filename.c_str() );
	return (mUzFile != NULL);
}

FileDataInterface* ZipArchive::FileRead(const std::string& _filename) const
{
    unz_file_info tagUFI;
    if(unzLocateFile( mUzFile, _filename.c_str(), 2) == UNZ_OK)
	{
        unzGetCurrentFileInfo(mUzFile, &tagUFI, NULL, 0, NULL, 0, NULL, 0);
		FileDataInterface* retData = new FileData(tagUFI.uncompressed_size);
		unzOpenCurrentFile(mUzFile);
        unzReadCurrentFile(mUzFile,(void*)retData->getBuffer(),tagUFI.uncompressed_size);
        unzCloseCurrentFile(mUzFile);
        return retData;
    }
    return NULL;
}

bool	ZipArchive::HaveFile(const std::string& _filename) const 
{
	if( unzLocateFile(mUzFile, _filename.c_str(), 2) == UNZ_OK)
	{
		return true;
	}
	return false;
}

int			ZipArchive::GetPriority() const
{
	return mPriority; 
}

std::string	ZipArchive::GetZipName() const 
{
	return mZipName; 
}