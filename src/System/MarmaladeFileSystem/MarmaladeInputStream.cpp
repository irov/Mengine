#	include "MarmaladeInputStream.h"

#	include "Interface/LogSystemInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeInputStream::MarmaladeInputStream()
        : m_serviceProvider(nullptr)
        , m_hFile(nullptr)
		, m_size(0)
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
	bool MarmaladeInputStream::open( const FilePath & _folder, const FilePath& _filename )
	{
        char filePath[260];
        strcpy( filePath, _folder.c_str() );
        strcat( filePath, _filename.c_str() );

        m_hFile = s3eFileOpen( filePath, "rb" );

        int32 s3e_size = s3eFileGetSize( m_hFile );

        if( s3e_size < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeInputStream::open %s invalid get size"
                , filePath
                );

            return false;
        }

        m_size = (size_t)s3e_size;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeInputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
			s3eFileClose( m_hFile );
			m_hFile = nullptr;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeInputStream::read( void* _buf, size_t _count )
	{     
        if( _count == m_size )
        {
            uint32 s3e_count = static_cast<uint32>(_count);
            uint32 bytesRead = s3eFileRead( _buf, 1, s3e_count, m_hFile );

            s3eFileError error = s3eFileGetError();
            if( bytesRead != S3E_FILE_ERR_NONE )
            {
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

            s3eFileError error = s3eFileGetError();
            if( error != S3E_FILE_ERR_NONE )
            {
                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read error %d:%d size %d get error %d"
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

        uint32 needRead = (m_reading + FILE_BUFFER_SIZE > m_size) ? _count: FILE_BUFFER_SIZE;

        uint32 bytesRead = ::s3eFileRead( m_buff, 1, needRead, m_hFile );

        s3eFileError error = s3eFileGetError();
        if( error != S3E_FILE_ERR_NONE )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read (%d:%d) size %d get error %d"
                , bytesRead
                , FILE_BUFFER_SIZE
                , m_size
                , error
                );

            return 0;
        }

        uint32 readSize = (std::min)( static_cast<uint32>(_count - tail), bytesRead );

        memcpy( (char *)_buf + tail, m_buff, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeInputStream::seek( size_t _pos )
	{
        if( _pos >= m_reading - m_capacity && _pos < m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
    		if( s3eFileSeek( m_hFile, static_cast<int32>(_pos), S3E_FILESEEK_SET ) != S3E_RESULT_SUCCESS )
            {
                s3eFileError error = s3eFileGetError();

                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::seek %d size %d get error %d"
                    , _pos
                    , m_size
                    , error
                    );

                return;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading = static_cast<uint32>(_pos);
        }
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
	bool MarmaladeInputStream::time( time_t & _time ) const
	{
        return false;
	}
}	// namespace Menge
