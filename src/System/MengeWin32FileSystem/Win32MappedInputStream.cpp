#	include "Win32MappedInputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Core/MemoryProxyInput.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32MappedInputStream::Win32MappedInputStream( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
        , m_hFile(INVALID_HANDLE_VALUE)
		, m_hMapping(INVALID_HANDLE_VALUE)
		, m_memory(0)
        , m_data(0)
        , m_pos(NULL)
        , m_end(NULL)
        , m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32MappedInputStream::~Win32MappedInputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * Win32MappedInputStream::createInputMemory()
    {
        MemoryProxyInput * memory = new MemoryProxyInput;

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MappedInputStream::openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size )
    {
        (void)_filename;

        MemoryProxyInput * memory = static_cast<MemoryProxyInput *>(_stream);

        unsigned char* pMem = static_cast<unsigned char*>(m_memory);

        memory->setMemory( pMem + _offset, _size );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32MappedInputStream::open( const FilePath & _filename )
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
            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open %ls invalid open"
                , unicode_filename.c_str()
                );

			return false;
		}

		if( m_size == INVALID_FILE_SIZE )
		{
			::CloseHandle( m_hFile );
			return false;
		}

		m_hMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

		if( m_hMapping == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open %ls invalid create file mapping"
                , unicode_filename.c_str()
                );

			::CloseHandle( m_hFile );

			return false;
		}

		m_memory = MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );

		if( m_memory == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open %ls invalid map view of file"
                , unicode_filename.c_str()
                );

			::CloseHandle( m_hMapping );
			::CloseHandle( m_hFile );

			return false;
		}

        m_size = GetFileSize( m_hFile, NULL );

        m_data = static_cast<unsigned char*>( m_memory );

        m_pos = m_data;
        m_end = m_data + m_size;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32MappedInputStream::destroy()
	{
        UnmapViewOfFile( m_memory );

		if( m_hMapping != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hMapping );
			m_hMapping = INVALID_HANDLE_VALUE;
		}

		if( m_hFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}

        delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32MappedInputStream::read( void* _buf, size_t _count )
	{
        size_t cnt = _count;
        // Read over end of memory?
        if ( m_pos + cnt > m_end )
        {
            cnt = m_end - m_pos;
        }

        if ( cnt == 0 )
        {
            return 0;
        }

        std::copy( m_pos, m_pos + cnt, static_cast<unsigned char*>(_buf) );

        m_pos += cnt;

        return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32MappedInputStream::seek( size_t _pos )
	{
        if( _pos < 0 )
        {
            _pos = 0;
        }
        else if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32MappedInputStream::tell() const
	{
        size_t distance = std::distance( m_data, m_pos );

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32MappedInputStream::size() const 
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
	bool Win32MappedInputStream::time( time_t & _time ) const
	{
		FILETIME creation;
		FILETIME access;
		FILETIME write;

		if( GetFileTime( m_hFile, &creation, &access, &write ) == FALSE )
		{
            DWORD dwError = GetLastError();

            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::time invalid get file time '%d'"
                , dwError
                );

			return false;
		}

		_time = s_FileTimeToUnixTime( &write );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//void * Win32MappedInputStream::getMemory() const 
	//{
	//	return m_memory;
	//}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
