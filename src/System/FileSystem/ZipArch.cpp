#	include "ZipArch.h"

#	include "FileData.h"

#	include "zlib\unzip.h"
#	include "zlib\iowin32.h" 

#	include <assert.h>

ZipArchive::ZipArchive(const std::string& _filename, int _prior) 
: m_uzFile(0),
m_priority(_prior)
{
	assert( _prior > 0 );
	if(!loadZip(_filename, _prior))
	{
		assert(!"error in FileSystem::ZipArchive()");
	}
} 

ZipArchive::~ZipArchive()
{
	unloadZip();
}

bool ZipArchive::unloadZip() 
{
	unzCloseCurrentFile(m_uzFile);
	int nRet = unzClose(m_uzFile);
	m_uzFile = NULL;
	return (nRet == UNZ_OK);
}

bool ZipArchive::loadZip( const std::string& _filename, int _prior ) 
{
	assert( !_filename.empty() );

	m_zipName = _filename;
	m_priority = _prior;
	unloadZip();
	m_uzFile = unzOpen( _filename.c_str() );
	return (m_uzFile != NULL);
}

FileDataInterface* ZipArchive::fileRead( const std::string& _filename ) const
{
    unz_file_info tagUFI;
    if(unzLocateFile( m_uzFile, _filename.c_str(), 2) == UNZ_OK)
	{
        unzGetCurrentFileInfo(m_uzFile, &tagUFI, NULL, 0, NULL, 0, NULL, 0);
		FileDataInterface* retData = new FileData(tagUFI.uncompressed_size);
		unzOpenCurrentFile(m_uzFile);
        unzReadCurrentFile(m_uzFile,(void*)retData->getBuffer(),tagUFI.uncompressed_size);
        unzCloseCurrentFile(m_uzFile);
        return retData;
    }
    return NULL;
}

bool ZipArchive::haveFile(const char* _filename) const
{
	if( unzLocateFile(m_uzFile, _filename, 2) == UNZ_OK)
	{
		return true;
	}
	return false;
}

int ZipArchive::getPriority() const
{
	return m_priority; 
}

std::string ZipArchive::getZipName() const
{
	return m_zipName; 
}