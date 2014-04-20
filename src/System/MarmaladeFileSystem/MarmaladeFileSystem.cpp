#	include "MarmaladeFileSystem.h"

#   include "Interface/MarmaladeLayerInterface.h"

#	include "MarmaladeInputStream.h"
#	include "MarmaladeOutputStream.h"
#	include "MarmaladeMappedInputStream.h"

#   include "Logger/Logger.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Menge::FileSystemInterface, Menge::MarmaladeFileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileSystem::MarmaladeFileSystem()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileSystem::~MarmaladeFileSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeFileSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeFileSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	FileInputStreamInterfacePtr MarmaladeFileSystem::createInputStream()
	{
		MarmaladeInputStream * inputStream = m_factoryInputStream.createObjectT();;

        inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
    //////////////////////////////////////////////////////////////////////////
    FileOutputStreamInterfacePtr MarmaladeFileSystem::createOutputStream()
    {
        MarmaladeOutputStream * outStream = m_factoryOutputStream.createObjectT();

        outStream->setServiceProvider( m_serviceProvider );

        return outStream;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedFileInterfacePtr MarmaladeFileSystem::createMappedFile()
    {
        MarmaladeMappedInputStream* inputStream = m_factoryMappedInputStream.createObjectT();

        inputStream->setServiceProvider( m_serviceProvider );

        //Do not support
        return inputStream;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::existFile( const FilePath & _folder, const FilePath & _fileName ) const
	{
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _fileName, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        if( s3eFileGetFileInt( filePath, S3E_FILE_ISFILE ) != 0 )
        {
            return true;
        }

        if( s3eFileCheckExists( filePath ) != S3E_FALSE )
        {
            return true;
        }

        return false;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeFileSystem::deleteFile( const FilePath & _folder, const FilePath& _filename )
    {
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::deleteFile invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( s3eFileDelete( filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeFileSystem::existFolder( const FilePath & _folder, const FilePath & _filename ) const
    {
        if( _folder.empty() == true && _filename.empty() == true )
        {
            return true;
        }

        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( s3eFileGetFileInt( filePath, S3E_FILE_ISDIR ) != 0 )
        {
            return true;
        }

        if( s3eFileCheckExists( filePath ) != S3E_FALSE )
        {
            return true;
        }

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::createFolder( const FilePath & _folder, const FilePath& _filename )
	{
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( s3eFileMakeDirectory( filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileSystem::deleteFolder( const FilePath & _folder, const FilePath& _filename )
	{
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( s3eFileDeleteDirectory( filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
}
