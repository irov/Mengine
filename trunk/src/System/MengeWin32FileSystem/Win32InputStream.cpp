#	include "Win32InputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::Win32InputStream( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , m_hFile(INVALID_HANDLE_VALUE)
		, m_size(0)
        , m_carriage(0)
        , m_capacity(0)
        , m_reading(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::~Win32InputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::open( const FilePath& _filename )
	{
        WString unicode_filename;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _filename.c_str(), _filename.size(), unicode_filename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %s invalid convert utf8 to unicode"
                , _filename.c_str()
                );

            return false;
        }

		m_hFile = WINDOWSLAYER_SERVICE(m_serviceProvider)->createFile( 
            unicode_filename, // file to open
			GENERIC_READ, // open for reading
			FILE_SHARE_READ, // share for reading, exclusive for mapping
			OPEN_EXISTING // existing file only
            );

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid open"
                , unicode_filename.c_str()
                );

			return false;
		}

		m_size = ::GetFileSize( m_hFile, NULL );

		if( m_size == INVALID_FILE_SIZE )
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;

            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid file size"
                , unicode_filename.c_str()
                );
            
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32InputStream::destroy()
	{
		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}

        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32InputStream::read( void* _buf, size_t _count )
	{     
        if( _count == m_size )
        {
            DWORD bytesRead = 0;
            if( ::ReadFile( m_hFile, _buf, static_cast<DWORD>( _count ), &bytesRead, NULL ) == FALSE )
            {
                DWORD dwError = GetLastError();

                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read %d:%d get error '%d'"
                    , _count
                    , m_size
                    , dwError
                    );

                return 0;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading += bytesRead;

            return bytesRead;
        }
        
        if( _count > FILE_BUFFER_SIZE )
        {            
            size_t tail = m_capacity - m_carriage;
            
            if( tail != 0 )
            {
                memcpy( _buf, m_buff + m_carriage, tail );
            }

            DWORD bytesRead = 0;
            if( ::ReadFile( m_hFile, (char *)_buf + tail, static_cast<DWORD>( _count - tail ), &bytesRead, NULL ) == FALSE )
            {
                DWORD dwError = GetLastError();

                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read %d:%d get error '%d'"
                    , _count - tail
                    , m_size
                    , dwError
                    );

                return 0;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading += bytesRead;

            return bytesRead + tail;
        }
        
        if( m_carriage + _count <= m_capacity )
        {
            memcpy( _buf, m_buff + m_carriage, _count );

            m_carriage += _count;

            return _count;
        }

        size_t tail = m_capacity - m_carriage;

        if( tail != 0 )
        {
            memcpy( _buf, m_buff + m_carriage, tail );
        }

        DWORD bytesRead = 0;
        if( ::ReadFile( m_hFile, m_buff, static_cast<DWORD>( FILE_BUFFER_SIZE ), &bytesRead, NULL ) == FALSE )
        {
            DWORD dwError = GetLastError();

            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read %d:%d get error '%d'"
                , FILE_BUFFER_SIZE
                , m_size
                , dwError
                );

            return 0;
        }
        
        DWORD readSize = (std::min)( (DWORD)(_count - tail), bytesRead );

        memcpy( (char *)_buf + tail, m_buff, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32InputStream::seek( size_t _pos )
	{
        if( _pos >= m_reading - m_capacity && _pos < m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
    		DWORD dwPtr = ::SetFilePointer( m_hFile, static_cast<LONG>( _pos ), NULL, FILE_BEGIN );

            if( dwPtr == INVALID_SET_FILE_POINTER )
            {
                DWORD dwError = GetLastError();

                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::seek %d:%d get error '%d'"
                    , _pos
                    , m_size
                    , dwError
                    );

                return;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading = dwPtr;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32InputStream::tell() const
	{
        size_t current = m_reading - m_capacity + m_carriage;

        return current;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32InputStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	static time_t s_FileTimeToUnixTime( const FILETIME * filetime )
	{
		unsigned int a0;			/* 16 bit, low    bits */
		unsigned int a1;			/* 16 bit, medium bits */
		unsigned int a2;			/* 32 bit, high   bits */

		unsigned int carry;		/* carry bit for subtraction */
		int negative;		/* whether a represents a negative value */

		/* Copy the time values to a2/a1/a0 */
		a2 = filetime->dwHighDateTime;
		a1 = ((unsigned int) filetime->dwLowDateTime) >> 16;
		a0 = ((unsigned int) filetime->dwLowDateTime) & 0xffff;

		/* Subtract the time difference */
		if (a0 >= 32768)
			a0 -= 32768, carry = 0;
		else
			a0 += (1 << 16) - 32768, carry = 1;

		if (a1 >= 54590 + carry)
			a1 -= 54590 + carry, carry = 0;
		else
			a1 += (1 << 16) - 54590 - carry, carry = 1;

		a2 -= 27111902 + carry;

		/* If a is negative, replace a by (-1-a) */
		negative = (a2 >= ((unsigned int) 1) << 31);
		if (negative)
		{
			/* Set a to -a - 1 (a is a2/a1/a0) */
			a0 = 0xffff - a0;
			a1 = 0xffff - a1;
			a2 = ~a2;
		}

		/* Divide a by 10000000 (a = a2/a1/a0), put the rest into r.
		Split the divisor into 10000 * 1000 which are both less than 0xffff. */
		a1 += (a2 % 10000) << 16;
		a2 /= 10000;
		a0 += (a1 % 10000) << 16;
		a1 /= 10000;
		a0 /= 10000;

		a1 += (a2 % 1000) << 16;
		a2 /= 1000;
		a0 += (a1 % 1000) << 16;
		a1 /= 1000;
		a0 /= 1000;

		/* If a was negative, replace a by (-1-a) and r by (9999999 - r) */
		if (negative)
		{
			/* Set a to -a - 1 (a is a2/a1/a0) */
			a0 = 0xffff - a0;
			a1 = 0xffff - a1;
			a2 = ~a2;
		}

		/* Do not replace this by << 32, it gives a compiler warning and it does
		not work. */
		return ((((time_t) a2) << 16) << 16) + (a1 << 16) + a0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::time( time_t & _time ) const
	{
		FILETIME creation;
		FILETIME access;
		FILETIME write;

		if( GetFileTime( m_hFile, &creation, &access, &write ) == FALSE )
        {
            DWORD dwError = GetLastError();

            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::time invalid get file time '%d'"
                , dwError
                );

			return false;
		}

		_time = s_FileTimeToUnixTime( &write );

		return true;
	}
}	// namespace Menge
