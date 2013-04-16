#	include "Win32OutputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32OutputStream::Win32OutputStream( ServiceProviderInterface * _serviceProvider )		
        : m_serviceProvider(_serviceProvider)
        , m_hFile(INVALID_HANDLE_VALUE)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32OutputStream::~Win32OutputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32OutputStream::open( const FilePath& _filename )
	{        
        WString unicode_filename;        
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _filename.c_str(), _filename.size(), unicode_filename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32OutputStream::open %s invalid convert utf8 to unicode"
                , _filename.c_str()
                );

            return false;
        }

		m_hFile = WINDOWSLAYER_SERVICE(m_serviceProvider)->createFile( 
            unicode_filename,    // file to open
			GENERIC_WRITE, // open for writing
			FILE_SHARE_READ | FILE_SHARE_WRITE,	// share for reading, exclusive for mapping
			CREATE_ALWAYS
            );

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
            LOGGER_ERROR(m_serviceProvider)("Win32OutputStream::open %ls invalid open"
                , unicode_filename.c_str()
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32OutputStream::destroy()
	{
		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}

        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32OutputStream::write( const void * _data, size_t _count )
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
	void Win32OutputStream::flush()
	{
		::FlushFileBuffers( m_hFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
