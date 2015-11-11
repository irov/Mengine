#	include "MarmaladeFileGroupDirectory.h"

#   include "Interface/PlatformInterface.h"
#   include "Interface/UnicodeInterface.h"

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
	void MarmaladeFileGroupDirectory::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MarmaladeFileGroupDirectory::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::initialize( const FilePath & _path )
	{
		m_path = _path;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileGroupDirectory::finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::isPacked() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & MarmaladeFileGroupDirectory::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr MarmaladeFileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		(void)_fileName;
		(void)_streaming;

		MarmaladeFileInputStream * inputStream = m_factoryInputStream.createObject();

        inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		(void)_streaming;

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
        MarmaladeFileOutputStream * outStream = m_factoryOutputStream.createObject();

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

		FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>(_stream);

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
    FileMappedInterfacePtr MarmaladeFileGroupDirectory::createMappedFile()
    {
		LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::createMappedFile not support"
			);

        return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeFileGroupDirectory::openMappedFile not support"
			);
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
        WChar filePath[MENGINE_MAX_PATH];
        if( PLATFORM_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		Char utf8_filePath[MENGINE_MAX_PATH];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( filePath, -1, utf8_filePath, MENGINE_MAX_PATH, nullptr );

		if( s3eFileGetFileInt( utf8_filePath, S3E_FILE_ISFILE ) != 0 )
        {
            return true;
        }

		if( s3eFileCheckExists( utf8_filePath ) != S3E_FALSE )
        {
            return true;
        }

        return false;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeFileGroupDirectory::removeFile( const FilePath& _filename )
    {
		WChar filePath[MENGINE_MAX_PATH];
		if( PLATFORM_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::deleteFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		Char utf8_filePath[MENGINE_MAX_PATH];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( filePath, -1, utf8_filePath, MENGINE_MAX_PATH, nullptr );

		if( s3eFileDelete( utf8_filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeFileGroupDirectory::existDirectory( const FilePath & _filename ) const
    {
		WChar filePath[MENGINE_MAX_PATH];
        if( PLATFORM_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		Char utf8_filePath[MENGINE_MAX_PATH];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( filePath, -1, utf8_filePath, MENGINE_MAX_PATH, nullptr );

		if( s3eFileGetFileInt( utf8_filePath, S3E_FILE_ISDIR ) != 0 )
        {
            return true;
        }

		if( s3eFileCheckExists( utf8_filePath ) != S3E_FALSE )
        {
            return true;
        }

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::createDirectory( const FilePath& _filename )
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( PLATFORM_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		Char utf8_filePath[MENGINE_MAX_PATH];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( filePath, -1, utf8_filePath, MENGINE_MAX_PATH, nullptr );

		if( s3eFileMakeDirectory( utf8_filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupDirectory::removeDirectory( const FilePath& _filename )
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( PLATFORM_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		Char utf8_filePath[MENGINE_MAX_PATH];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( filePath, -1, utf8_filePath, MENGINE_MAX_PATH, nullptr );

		if( s3eFileDeleteDirectory( utf8_filePath ) != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		return true;
	}
}
