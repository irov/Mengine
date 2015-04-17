#	include "Win32FileGroupDirectory.h"

#	include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

#	include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupDirectory::Win32FileGroupDirectory()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupDirectory::~Win32FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileGroupDirectory::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * Win32FileGroupDirectory::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::initialize( const FilePath & _path )
	{
        m_path = _path;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::finalize()
    {
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::isPacked() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & Win32FileGroupDirectory::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFile invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->fileExists( filePath );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		(void)_fileName;
		(void)_streaming;

		Win32FileInputStream * inputStream = m_factoryInputStream.createObjectT();

		inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		(void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface>(_stream);

		if( file->open( m_path, _fileName, _offset, _size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile()
	{
		Win32FileOutputStream * outputStream = m_factoryOutputStream.createObjectT();

		outputStream->setServiceProvider( m_serviceProvider );

		return outputStream;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool Win32FileGroupDirectory::openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream )
	{
        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface>(_stream);

        if( file->open( m_path, _fileName ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }
        		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	FileMappedInterfacePtr Win32FileGroupDirectory::createMappedFile()
	{
		Win32FileMapped * mappedStream = m_factoryWin32MappedFile.createObjectT();

		mappedStream->setServiceProvider( m_serviceProvider );

		return mappedStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream )
	{
		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openMappedFile failed _stream == NULL"
				);

			return false;
		}

		if( _stream->open( m_path, _fileName ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openMappedFile failed open file '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _path ) const
    {
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _path, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFolder invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _path.c_str()
				);

			return false;
		}

		bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->fileExists( filePath );

		return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::createDirectory( const FilePath & _path )
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _path, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::createFolder invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _path.c_str()
				);

			return false;
		}

		bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->createDirectory( filePath );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::removeDirectory( const FilePath & _path )
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _path, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _path.c_str()
				);

			return false;
		}

		//Double Zero!
		size_t fp_len = wcslen(filePath);
		filePath[fp_len] = L'\0';
		filePath[fp_len + 1] = L'\0';

		SHFILEOPSTRUCT fileop;
		ZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));
		fileop.hwnd = NULL;
		fileop.wFunc = FO_DELETE;
		fileop.pFrom = filePath;
		fileop.pTo = NULL;		
		fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		fileop.fAnyOperationsAborted = FALSE;
		fileop.lpszProgressTitle = NULL;
		fileop.hNameMappings = NULL;

		int err = ::SHFileOperation( &fileop );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder %ls error %d"
				, filePath
				, err
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::removeFile( const FilePath & _fileName )
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFile invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTW));
		fs.hwnd = NULL;
		fs.pFrom = filePath;
		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;

		int err = ::SHFileOperation( &fs );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFile %s:%s error %d"
				, m_path.c_str()
				, _fileName.c_str()
				, err
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
