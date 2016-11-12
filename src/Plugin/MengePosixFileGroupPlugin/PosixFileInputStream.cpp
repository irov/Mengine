#	include "PosixFileInputStream.h"

#	include "Interface/SDLLayerInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Utils/Logger/Logger.h"

#	include <sys/stat.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileInputStream::PosixFileInputStream()
        : m_serviceProvider(nullptr)
        , m_hFile(nullptr)
		, m_size(0)
		, m_offset(0)
		, m_carriage(0)
		, m_capacity(0)
		, m_reading(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixFileInputStream::~PosixFileInputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void PosixFileInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size )
	{
		m_folder = _folder;
		m_filename = _fileName;

		Char filePath[MENGINE_MAX_PATH];
		if( this->concatenateFilePath( _folder, _fileName, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::open invalid concatenate '%s':'%s'"
				, m_folder.c_str()
				, m_filename.c_str()
				);

			return false;
		}

        m_hFile = fopen( filePath, "rb" );

        if( m_hFile == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::open s3eFileOpen %s:%s get error"
                , m_folder.c_str()
                , m_filename.c_str()
                );

            return false;
        }

		fseek( m_hFile, 0, SEEK_END );
		long size = (size_t)ftell( m_hFile );
		rewind( m_hFile );

		if( _offset + _size > (size_t)size )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::open %ls invalid file range %d:%d size %d"
				, filePath
				, _offset
				, _size
				, size
				);

			return false;
		}

		m_size = _size == 0 ? (size_t)size : _size;
		m_offset = _offset;
		
		m_carriage = 0;
		m_capacity = 0;
		m_reading = 0;

		if( m_offset != 0 )
		{
			this->seek( 0 );
		}	

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileInputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
			fclose( m_hFile );
			m_hFile = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PosixFileInputStream::read( void * _buf, size_t _count )
	{     
		size_t pos = m_reading - m_capacity + m_carriage;

		size_t correct_count = _count;

		if( pos + _count > m_size )
		{
			correct_count = m_size - pos;
		}

		if( correct_count == m_size )
		{
			size_t bytesRead;
			if( this->read_( _buf, correct_count, bytesRead ) == false )
			{
				return 0;
			}

			m_carriage = 0;
			m_capacity = 0;

			m_reading += bytesRead;

			return bytesRead;
		}

		if( correct_count > MENGINE_POSIX_FILE_BUFFER_SIZE )
		{            
			size_t tail = m_capacity - m_carriage;

			if( tail != 0 )
			{
				stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
			}

			size_t toRead = correct_count - tail;
			void * toBuffer = (uint8_t *)_buf + tail;

			size_t bytesRead;
			if( this->read_( toBuffer, toRead, bytesRead ) == false )
			{
				return 0;
			}

			m_carriage = 0;
			m_capacity = 0;

			m_reading += bytesRead;

			return bytesRead + tail;
		}

		if( m_carriage + correct_count <= m_capacity )
		{
			stdex::memorycopy( _buf, 0, m_readCache + m_carriage, correct_count );

			m_carriage += correct_count;

			return correct_count;
		}

		size_t tail = m_capacity - m_carriage;

		if( tail != 0 )
		{
			stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
		}

		size_t bytesRead;
		if( this->read_( m_readCache, MENGINE_POSIX_FILE_BUFFER_SIZE, bytesRead ) == false )
		{
			return 0;
		}

		size_t readSize = (std::min)(correct_count - tail, bytesRead);

		stdex::memorycopy( _buf, tail, m_readCache, readSize );

		m_carriage = readSize;
		m_capacity = bytesRead;

		m_reading += bytesRead;

		return readSize + tail;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::read_( void * _buf, size_t _size, size_t & _read )
	{
		size_t bytesRead = fread( _buf, 1, _size, m_hFile );

		_read = bytesRead;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::seek( size_t _pos )
	{
		if( _pos >= m_reading - m_capacity && _pos < m_reading )
		{
			m_carriage = m_capacity - (m_reading - _pos);
		}
		else
		{
			int result = fseek( m_hFile, static_cast<long>(m_offset + _pos), SEEK_SET );

			if( result != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::seek %d:%d size %d get error"
					, m_offset
					, _pos
					, m_size
					);

				return false;
			}

			m_carriage = 0;
			m_capacity = 0;

			m_reading = _pos;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::skip( size_t _pos )
	{
		size_t current = m_reading - m_capacity + m_carriage;

		size_t seek_pos = current + _pos;

		bool result = this->seek( seek_pos );

		return result;			
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PosixFileInputStream::tell() const
	{
		size_t current = m_reading - m_capacity + m_carriage;

		return current;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PosixFileInputStream::size() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::eof() const
	{
		return (m_reading - m_capacity + m_carriage) == m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::time( uint64_t & _time ) const
	{
		if( m_hFile == nullptr )
		{
			return false;
		}

		char filePath[MENGINE_MAX_PATH];
		if( this->concatenateFilePath( m_folder, m_filename, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PosixFileGroupDirectory::time invalid concatenate '%s':'%s'"
				, m_folder.c_str()
				, m_filename.c_str()
				);

			return false;
		}

		struct stat sb;
		if( stat( filePath, &sb ) != 0 )
		{
			return false;
		}

		time_t res = sb.st_mtime;
				
		_time = (uint64_t)res;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileInputStream::memory( void ** _memory, size_t * _size )
	{
		(void)_memory;
		(void)_size;

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    bool PosixFileInputStream::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity ) const
    {
        size_t folderSize = _folder.size();
        size_t fileNameSize = _fileName.size();

        if( folderSize + fileNameSize > _capacity )
        {
            return false;
        }

        strcpy( _filePath, _folder.c_str() );
        strcat( _filePath, _fileName.c_str() );

        return true;
    }
}	// namespace Menge
