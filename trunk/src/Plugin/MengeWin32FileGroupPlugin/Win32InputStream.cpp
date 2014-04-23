#	include "Win32InputStream.h"

#	include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::Win32InputStream()
        : m_serviceProvider(nullptr)
        , m_hFile(INVALID_HANDLE_VALUE)
		, m_size(0)
		, m_offset(0)
		, m_pos(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32InputStream::~Win32InputStream()
	{
        this->close_();
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32InputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32InputStream::close_()
    {
        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size )
	{
		WChar filePath[MAX_PATH];
		if( this->openFile_( _folder, _fileName, filePath ) == false )
		{
			return false;
		}

		DWORD size = ::GetFileSize( m_hFile, NULL );

		if( size == INVALID_FILE_SIZE )
		{
			this->close_();

			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid file size"
				, filePath
				);

			return false;
		}

		if( _offset + _size > size )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid file range %d:%d size %d"
				, filePath
				, _offset
				, _size
				, size
				);

			return false;
		}

		m_size = _size == 0 ? (size_t)size : _size;
		m_offset = _offset;
		m_pos = 0;

		if( m_offset != 0 )
		{
			this->seek( 0 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::openFile_( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath )
	{		
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( _folder, _fileName, _filePath, MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open invlalid concatenate filePath '%s':'%s'"
				, _folder.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		m_hFile = WINDOWSLAYER_SERVICE(m_serviceProvider)->createFile( 
			_filePath, // file to open
			GENERIC_READ, // open for reading
			FILE_SHARE_READ, // share for reading, exclusive for mapping
			OPEN_EXISTING // existing file only
			);

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid open"
				, _filePath
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32InputStream::read( void * _buf, size_t _count )
	{   
		if( m_pos + _count > m_size )
		{
			_count = m_size - m_pos;
		}

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

		m_pos += (size_t)bytesRead;

		return bytesRead;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32InputStream::seek( size_t _pos )
	{
   		DWORD dwPtr = ::SetFilePointer( m_hFile, static_cast<LONG>( m_offset + _pos ), NULL, FILE_BEGIN );

		if( dwPtr == INVALID_SET_FILE_POINTER )
		{
			DWORD dwError = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::seek %d:%d size %d get error '%d'"
				, m_offset
				, _pos
				, m_size
				, dwError
				);

			return false;
		}

		m_pos = (size_t)dwPtr - m_offset;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32InputStream::tell() const
	{
		//DWORD dwPtr = SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT );

  //      return (size_t)dwPtr - m_offset;

		return m_pos;
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
	bool Win32InputStream::time( uint64_t & _time ) const
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

        time_t time = s_FileTimeToUnixTime( &write );

		_time = (uint64_t)time;

		return true;
	}
}	// namespace Menge
