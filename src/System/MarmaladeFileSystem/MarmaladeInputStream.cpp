#	include "MarmaladeInputStream.h"

#	include "Interface/MarmaladeLayerInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeInputStream::MarmaladeInputStream()
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
	MarmaladeInputStream::~MarmaladeInputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeInputStream::open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size )
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

		m_offset = _offset;

		if( _size == 0 )
		{
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

			m_size = (size_t)s3e_size;
		}
		else
		{
			m_size = _size;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeInputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
			s3eFileClose( m_hFile );
			m_hFile = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeInputStream::read( void* _buf, size_t _count )
	{     
        if( _count == m_size )
        {
            uint32 s3e_count = static_cast<uint32>(_count);
            uint32 bytesRead = s3eFileRead( _buf, 1, s3e_count, m_hFile );

            if( bytesRead != s3e_count )
            {
                s3eFileError error = s3eFileGetError();

                LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::read (%d:%d) size %d get error %d"
                    , bytesRead
                    , _count
                    , m_size
                    , error
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

            uint32 read_count = static_cast<uint32>( _count - tail );
            uint32 bytesRead = s3eFileRead( (char *)_buf + tail, 1, read_count, m_hFile );

            if( bytesRead != read_count )
            {
                s3eFileError error = s3eFileGetError();

                LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::read error %d:%d size %d get error %d"
                    , bytesRead
                    , read_count
                    , m_size
                    , error
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

        //uint32 needRead = (m_reading + FILE_BUFFER_SIZE > m_size) ? _count: FILE_BUFFER_SIZE;
		// Fixes libpng issue, when buffer size is requested to be read when we have less bytes in file remaining.
        uint32 needRead = (m_reading + FILE_BUFFER_SIZE > m_size) ? std::min(_count, m_size - m_reading) : FILE_BUFFER_SIZE;

        uint32 bytesRead = ::s3eFileRead( m_buff, 1, needRead, m_hFile );

        if( bytesRead != needRead )
        {
            s3eFileError error = s3eFileGetError();

            LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::read (%d:%d) size %d get error %d"
                , bytesRead
                , FILE_BUFFER_SIZE
                , m_size
                , error
                );

            return 0;
        }

        uint32 readSize = (std::min)( static_cast<uint32>(_count - tail), needRead );

        memcpy( (char *)_buf + tail, m_buff, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeInputStream::seek( size_t _pos )
	{
        if( _pos >= m_reading - m_capacity && _pos < m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            s3eResult result = s3eFileSeek( m_hFile, static_cast<int32>(_pos), S3E_FILESEEK_SET );

    		if( result != S3E_RESULT_SUCCESS )
            {
                s3eFileError error = s3eFileGetError();

                LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::seek %d size %d get error %d"
                    , _pos
                    , m_size
                    , error
                    );

                return false;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading = static_cast<uint32>(_pos);
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeInputStream::tell() const
	{
        size_t current = m_reading - m_capacity + m_carriage;

        return current;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeInputStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeInputStream::time( uint64 & _time ) const
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
}	// namespace Menge
