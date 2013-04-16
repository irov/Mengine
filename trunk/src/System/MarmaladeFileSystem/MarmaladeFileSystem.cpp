#	include "MarmaladeFileSystem.h"

#	include "MarmaladeInputStream.h"
#	include "MarmaladeOutputStream.h"
#	include "MarmaladeMappedInputStream.h"

#   include "Logger/Logger.h"

#	include <algorithm>

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool createFileSystem( Menge::FileSystemInterface ** _fileSystem )
    {
        if( _fileSystem == 0 )
        {
            return false;
        }

        *_fileSystem = new Menge::MarmaladeFileSystem;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void destroyFileSystem( Menge::FileSystemInterface * _fileSystem )
    {
        if( _fileSystem != 0 )
        {
            delete static_cast<Menge::MarmaladeFileSystem*>(_fileSystem);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileSystem::MarmaladeFileSystem()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileSystem::~MarmaladeFileSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeFileSystem::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* MarmaladeFileSystem::createInputStream()
	{
		MarmaladeInputStream* inputStream = new MarmaladeInputStream(m_serviceProvider);

		return inputStream;
	}
    //////////////////////////////////////////////////////////////////////////
    FileOutputStreamInterface* MarmaladeFileSystem::createOutputStream()
    {
        MarmaladeOutputStream* outStream = new MarmaladeOutputStream(m_serviceProvider);

        return outStream;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedFileInputStreamInterface * MarmaladeFileSystem::createMappedInputStream()
    {
        //MarmaladeMappedInputStream* inputStream = new MarmaladeMappedInputStream(m_serviceProvider);

        //Do not support
        return NULL;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::existFile( const FilePath& _filename ) const
	{
        if( s3eFileGetFileInt( _filename.c_str(), S3E_FILE_ISDIR ) != 0 )
        {
            return true;
        }

        if( s3eFileCheckExists( _filename.c_str() ) != S3E_FALSE )
        {
            return true;
        }

        return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::createFolder( const FilePath& _path )
	{
        if( s3eFileMakeDirectory( _path.c_str() ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::deleteFolder( const FilePath& _path )
	{
        if( s3eFileDeleteDirectory( _path.c_str() ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::deleteFile( const FilePath& _filename )
	{
        if( s3eFileDelete( _filename.c_str() ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}

	ServiceProviderInterface * MarmaladeFileSystem::getServiceProvider() const
	{
		return m_serviceProvider;
	}
}
