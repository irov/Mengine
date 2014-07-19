#	include "MarmaladeFileGroupDirectory.h"

#   include "Interface/MarmaladeLayerInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileGroupDirectory::MarmaladeFileGroupDirectory()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileGroupDirectory::~MarmaladeFileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path )
	{
		m_serviceProvider = _serviceProvider;

		m_path = _path;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileGroupDirectory::finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & MarmaladeFileGroupDirectory::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr MarmaladeFileGroupDirectory::createInputFile( const FilePath & _fileName )
	{
		(void)_fileName;

		MarmaladeInputStream * inputStream = m_factoryInputStream.createObjectT();

        inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
	{
		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openInputFile failed _stream == NULL"
				);

			return false;
		}

		FileInputStreamInterfacePtr file = stdex::intrusive_static_cast<FileInputStreamInterfacePtr>(_stream);

		if( file->open( m_path, _fileName, _offset, _size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openInputFile failed open file '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr MarmaladeFileGroupDirectory::createOutputFile()
    {
        MarmaladeOutputStream * outStream = m_factoryOutputStream.createObjectT();

        outStream->setServiceProvider( m_serviceProvider );

        return outStream;
    }
	//////////////////////////////////////////////////////////////////////////	
	bool MarmaladeFileGroupDirectory::openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream )
	{
		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openOutputFile failed _stream == NULL"
				);

			return false;
		}

		FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface>(_stream);

		if( file->open( m_path, _fileName ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openOutputFile failed open file '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    MappedFileInterfacePtr MarmaladeFileGroupDirectory::createMappedFile()
    {
		LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::createMappedFile not support"
			);

        return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::openMappedFile( const FilePath & _fileName, const MappedFileInterfacePtr & _stream )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openMappedFile not support"
			);
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
        Char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _fileName, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
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
    bool MarmaladeFileGroupDirectory::removeFile( const FilePath& _filename )
    {
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::deleteFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
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
    bool MarmaladeFileGroupDirectory::existDirectory( const FilePath & _filename ) const
    {
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
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
	bool MarmaladeFileGroupDirectory::createDirectory( const FilePath& _filename )
	{
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
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
	bool MarmaladeFileGroupDirectory::removeDirectory( const FilePath& _filename )
	{
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
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
