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
	bool Win32FileSystem::existFile( const FilePath& _filename ) const
	{
        static WString unicode_filename;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _filename.c_str(), _filename.size(), unicode_filename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFile invlalid convert utf8 to unicode %s"
                , _filename.c_str()
                );

            return false;
        }

        bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->fileExists( unicode_filename );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::createFolder( const FilePath& _path )
	{
        static WString unicode_path;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _path.c_str(), _path.size(), unicode_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::createFolder invlalid convert utf8 to unicode %s"
                , _path.c_str()
                );

            return false;
        }
        
        bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
            ->createDirectory( unicode_path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFolder( const FilePath& _path )
	{
        static WString unicode_path;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _path.c_str(), _path.size(), unicode_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder invlalid convert utf8 to unicode %s"
                , _path.c_str()
                );

            return false;
        }

        unicode_path += L" ";
        unicode_path[unicode_path.size() - 1] = L'\0';

		SHFILEOPSTRUCT fileop;
		ZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));
		fileop.hwnd = NULL;
        fileop.wFunc = FO_DELETE;
		fileop.pFrom = unicode_path.c_str();
        fileop.pTo = NULL;		
		fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
        fileop.fAnyOperationsAborted = FALSE;
        fileop.lpszProgressTitle = NULL;
        fileop.hNameMappings = NULL;
		
		int err = ::SHFileOperation( &fileop );
		
		if( err != 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFolder %s error %d"
                , _path.c_str()
                , err
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFile( const FilePath& _filename )
	{
        static WString unicode_filename;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _filename.c_str(), _filename.size(), unicode_filename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::deleteFile invlalid convert utf8 to unicode %s"
                , _filename.c_str()
                );

            return false;
        }

		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTW));
		fs.hwnd = NULL;
		fs.pFrom = unicode_filename.c_str();
		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		
		if( ::SHFileOperation( &fs ) != 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
