#	include "Win32FileOutputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileOutputStream::Win32FileOutputStream()		
        : m_serviceProvider(nullptr)
        , m_hFile(INVALID_HANDLE_VALUE)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileOutputStream::~Win32FileOutputStream()
	{
        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32FileOutputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileOutputStream::open( const FilePath & _folder, const FilePath& _fileName )
	{        
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( _folder, _fileName, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32OutputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		m_hFile = WINDOWSLAYER_SERVICE(m_serviceProvider)->createFile( 
            filePath
            , GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , CREATE_ALWAYS
            );

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
            LOGGER_ERROR(m_serviceProvider)("Win32OutputStream::open %ls invalid open"
                , filePath
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileOutputStream::write( const void * _data, size_t _count )
	{
		DWORD bytesWritten = 0;
		BOOL result = ::WriteFile( m_hFile, _data, _count, &bytesWritten, NULL );

        if( result == FALSE )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32OutputStream::write invalid %d"
                , _count
                );

            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileOutputStream::flush()
	{
		if( ::FlushFileBuffers( m_hFile ) == FALSE )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
