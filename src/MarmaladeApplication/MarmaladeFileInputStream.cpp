#	include "MarmaladeFileInputStream.h"

#	include "Interface/MarmaladeLayerInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileInputStream::MarmaladeFileInputStream()
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
	MarmaladeFileInputStream::~MarmaladeFileInputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeFileInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size )
	{
		m_folder = _folder;
		m_filename = _fileName;

		Char filePath[MAX_PATH];
		if( MARMALADELAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( _folder, _fileName, filePath, MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::open invalid concatenate '%s':'%s'"
				, m_folder.c_str()
				, m_filename.c_str()
				);

			return false;
		}

        m_hFile = s3eFileOpen( filePath, "rb" );

        if( m_hFile == nullptr )
        {
            s3eFileError error = s3eFileGetError();

            LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::open s3eFileOpen %s:%s get error %d"
                , m_folder.c_str()
                , m_filename.c_str()
                , error
                );

            return false;
        }

		int32 s3e_size = s3eFileGetSize( m_hFile );

		if( s3e_size == -1 )
		{
			s3eFileError error = s3eFileGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::open s3eFileGetSize %s:%s get error %d"
				, m_folder.c_str()
				, m_filename.c_str()
				, error
				);

			return false;
		}

		if( _offset + _size > (uint32)s3e_size )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32InputStream::open %ls invalid file range %d:%d size %d"
				, filePath
				, _offset
				, _size
				, s3e_size
				);

			return false;
		}

		m_size = _size == 0 ? (size_t)s3e_size : _size;
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
	void MarmaladeFileInputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
			s3eFileClose( m_hFile );
			m_hFile = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeFileInputStream::read( void * _buf, size_t _count )
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

		if( correct_count > MENGINE_MARMALADE_FILE_BUFFER_SIZE )
		{            
			size_t tail = m_capacity - m_carriage;

			if( tail != 0 )
			{
				stdex::memorycopy( _buf, m_readCache + m_carriage, tail );
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
			stdex::memorycopy( _buf, m_readCache + m_carriage, correct_count );

			m_carriage += correct_count;

			return correct_count;
		}

		size_t tail = m_capacity - m_carriage;

		if( tail != 0 )
		{
			stdex::memorycopy( _buf, m_readCache + m_carriage, tail );
		}

		size_t bytesRead;
		if( this->read_( m_readCache, MENGINE_MARMALADE_FILE_BUFFER_SIZE, bytesRead ) == false )
		{
			return 0;
		}

		size_t readSize = (std::min)(correct_count - tail, bytesRead);

		unsigned char * read_buf = (unsigned char *)_buf + tail;
		stdex::memorycopy( read_buf, m_readCache, readSize );

		m_carriage = readSize;
		m_capacity = bytesRead;

		m_reading += bytesRead;

		return readSize + tail;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::read_( void * _buf, size_t _size, size_t & _read )
	{
		uint32 s3e_count = static_cast<uint32>(_size);
		uint32 bytesRead = s3eFileRead( _buf, 1, s3e_count, m_hFile );

		_read = (size_t)bytesRead;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::seek( size_t _pos )
	{
		if( _pos >= m_reading - m_capacity && _pos < m_reading )
		{
			m_carriage = m_capacity - (m_reading - _pos);
		}
		else
		{
			s3eResult result = s3eFileSeek( m_hFile, static_cast<int32>(m_offset + _pos), S3E_FILESEEK_SET );

			if( result != S3E_RESULT_SUCCESS )
			{
				s3eFileError error = s3eFileGetError();

				LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::seek %d:%d size %d get error %d"
					, m_offset
					, _pos
					, m_size
					, error
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
	bool MarmaladeFileInputStream::skip( size_t _pos )
	{
		size_t current = m_reading - m_capacity + m_carriage;

		size_t seek_pos = current + _pos;

		bool result = this->seek( seek_pos );

		return result;			
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeFileInputStream::tell() const
	{
		size_t current = m_reading - m_capacity + m_carriage;

		return current;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeFileInputStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::eof() const
	{
		return (m_reading - m_capacity + m_carriage) == m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::time( uint64 & _time ) const
	{
		if( m_hFile == nullptr )
		{
			return false;
		}

		char filePath[MAX_PATH];
		if( MARMALADELAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_folder, m_filename, filePath, MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::time invalid concatenate '%s':'%s'"
				, m_folder.c_str()
				, m_filename.c_str()
				);

			return false;
		}

		uint64 res = s3eFileGetFileInt( filePath, S3E_FILE_MODIFIED_DATE );
		
		if( res == (uint64)-1 )
		{
			s3eFileError err = s3eFileGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::open invalid get file modified '%s':'%s' - '%d'"
				, m_folder.c_str()
				, m_filename.c_str()
				, err
				);

			return false;
		}
		
		_time = res;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileInputStream::memory( void ** _memory, size_t * _size )
	{
		(void)_memory;
		(void)_size;

		return false;
	}
}	// namespace Menge
