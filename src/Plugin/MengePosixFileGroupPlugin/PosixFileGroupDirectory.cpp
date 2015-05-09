#	include "PosixFileGroupDirectory.h"

#   include "Interface/SDLLayerInterface.h"

#   include "Logger/Logger.h"

#include <sys/stat.h>
#include <direct.h>

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISNDIR
#define S_ISNDIR(x) (((x) & S_IFMT) != S_IFDIR)
#endif


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileGroupDirectory::PosixFileGroupDirectory()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixFileGroupDirectory::~PosixFileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileGroupDirectory::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * PosixFileGroupDirectory::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::initialize( const FilePath & _path )
	{
		m_path = _path;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileGroupDirectory::finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::isPacked() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & PosixFileGroupDirectory::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr PosixFileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		(void)_fileName;
		(void)_streaming;

		PosixFileInputStream * inputStream = m_factoryInputStream.createObjectT();

        inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		(void)_streaming;

		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::openInputFile failed _stream == NULL"
				);

			return false;
		}

		FileInputStreamInterfacePtr file = stdex::intrusive_static_cast<FileInputStreamInterfacePtr>(_stream);

		if( file->open( m_path, _fileName, _offset, _size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::openInputFile failed open file '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr PosixFileGroupDirectory::createOutputFile()
    {
        PosixFileOutputStream * outStream = m_factoryOutputStream.createObjectT();

        outStream->setServiceProvider( m_serviceProvider );

        return outStream;
    }
	//////////////////////////////////////////////////////////////////////////	
	bool PosixFileGroupDirectory::openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream )
	{
		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::openOutputFile failed _stream == NULL"
				);

			return false;
		}

		FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface>(_stream);

		if( file->open( m_path, _fileName ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::openOutputFile failed open file '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	FileMappedInterfacePtr PosixFileGroupDirectory::createMappedFile()
    {
		LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::createMappedFile not support"
			);

        return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream )
	{
		(void)_fileName;
		(void)_stream;

		LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::openMappedFile not support"
			);
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
        Char filePath[MENGINE_MAX_PATH];
		if( SDLLAYER_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		struct stat sb;
		if( stat( filePath, &sb ) == 0 && S_ISNDIR( sb.st_mode ) )
		{
			return true;
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::removeFile( const FilePath& _filename )
    {
        char filePath[MENGINE_MAX_PATH];
		if( SDLLAYER_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::deleteFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		if( remove( filePath ) != -1 )
		{
			return true;
		}

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::existDirectory( const FilePath & _filename ) const
    {
        char filePath[MENGINE_MAX_PATH];
		if( SDLLAYER_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::existFile invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

		struct stat sb;
		if( stat( filePath, &sb ) == 0 && S_ISDIR( sb.st_mode ) )
		{
			return true;
		}

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::createDirectory( const FilePath& _filename )
	{
        char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

#if _WIN32
		return _mkdir( filePath ) != -1;
#else
		return mkdir( filePath, 0777 ) != -1;
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupDirectory::removeDirectory( const FilePath& _filename )
	{
        char filePath[MENGINE_MAX_PATH];
		if( SDLLAYER_SERVICE( m_serviceProvider )
            ->concatenateFilePath( m_path, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeFileSystem::createFolder invalid concatenate '%s':'%s'"
                , m_path.c_str()
                , _filename.c_str()
                );

            return false;
        }

#if _WIN32
		return _rmdir( filePath ) != -1;
#else
		return rmdir( filePath, 0777 ) != -1;
#endif
	}
}
