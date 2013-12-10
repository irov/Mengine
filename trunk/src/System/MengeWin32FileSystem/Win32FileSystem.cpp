#	include "Win32FileSystem.h"

#	include <algorithm>

#	include <shlobj.h>
#	include <ShellAPI.h>

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Menge::FileSystemInterface, Menge::Win32FileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::Win32FileSystem()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::~Win32FileSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32FileSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Win32FileSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	FileInputStreamInterfacePtr Win32FileSystem::createInputStream()
	{
        Win32InputStream * inputStream = m_factoryInputStream.createObjectT();

        inputStream->setServiceProvider( m_serviceProvider );
		
		return inputStream;
	}
    //////////////////////////////////////////////////////////////////////////
    FileOutputStreamInterfacePtr Win32FileSystem::createOutputStream()
    {
        Win32OutputStream * outputStream = m_factoryOutputStream.createObjectT();

        outputStream->setServiceProvider( m_serviceProvider );

        return outputStream;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedFileInputStreamInterfacePtr Win32FileSystem::createMappedInputStream()
    {
        Win32MappedInputStream * mappedStream = m_factoryMappedInputStream.createObjectT();

        mappedStream->setServiceProvider( m_serviceProvider );

        return mappedStream;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::existFile( const FilePath & _folder, const FilePath& _dir, const char * _filename, size_t _filenamelen ) const
	{
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _dir, _filename, _filenamelen, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFile invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _dir.c_str()
                );

            return false;
        }

        bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->fileExists( filePath );

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::existFolder( const FilePath & _folder, const FilePath& _filename  ) const
    {
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, ConstString::none(), _filename.c_str(), _filename.size(), filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFolder invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->fileExists( filePath );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::createFolder( const FilePath & _folder, const FilePath& _filename )
	{
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, ConstString::none(), _filename.c_str(), _filename.size(), filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::createFolder invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }
        
        bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->createDirectory( filePath );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFolder( const FilePath & _folder, const FilePath& _filename )
	{
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, ConstString::none(), _filename.c_str(), _filename.size(), filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        size_t fp_len = wcslen(filePath);
        filePath[fp_len] = L'\0';

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
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder %s error %d"
                , _filename.c_str()
                , err
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFile( const FilePath & _folder, const FilePath& _filename )
	{
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, ConstString::none(), _filename.c_str(), _filename.size(), filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFile invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
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
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFile %s error %d"
                , _filename.c_str()
                , err
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
