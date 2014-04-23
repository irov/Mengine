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
		, m_pos(0)
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

		if( s3e_size - (_offset + _size) < 0 )
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
		m_pos = 0;

		if( m_offset != 0 )
		{
			this->seek( 0 );
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
	size_t MarmaladeInputStream::read( void * _buf, size_t _count )
	{     
		if( m_pos + _count > m_size )
		{
			_count = m_size - m_pos;
		}

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

		m_pos += (size_t)bytesRead;

		return bytesRead;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeInputStream::seek( size_t _pos )
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

		m_pos = (size_t)_pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MarmaladeInputStream::tell() const
	{
        //int32 current = s3eFileTell( m_hFile );

        return m_pos;
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
