#	include "MarmaladeFileOutputStream.h"

#	include "Interface/MarmaladeLayerInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileOutputStream::MarmaladeFileOutputStream()		
        : m_serviceProvider(nullptr)
        , m_hFile(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileOutputStream::~MarmaladeFileOutputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeFileOutputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileOutputStream::open( const FilePath & _folder, const FilePath& _filename )
	{        
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeOutputStream::open invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        m_hFile = s3eFileOpen( filePath, "wb" );

        if( m_hFile == NULL )
        {
			const char * error_str = s3eFileGetErrorString();
            s3eFileError error = s3eFileGetError();			

            LOGGER_ERROR(m_serviceProvider)("MarmaladeOutputStream::open %s:%s get error %s [%d]"
                , _folder.c_str()
                , _filename.c_str()
				, error_str
                , error
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileOutputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
            s3eFileFlush( m_hFile );
			s3eFileClose( m_hFile );
			m_hFile = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileOutputStream::write( const void * _data, size_t _count )
	{
        uint32 bytesWritten = s3eFileWrite( _data, 1, _count, m_hFile );

        if( bytesWritten != _count )
        {
			const char * error_str = s3eFileGetErrorString();
            s3eFileError error = s3eFileGetError();
            
            LOGGER_ERROR(m_serviceProvider)("MarmaladeOutputStream::write %d:%d get error %s [%d]"
                , bytesWritten
                , _count
				, error_str
                , error
                );

            return false;        
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileOutputStream::flush()
	{
		s3eResult result = s3eFileFlush( m_hFile );

        if( result != S3E_RESULT_SUCCESS )
        {
			const char * error_str = s3eFileGetErrorString();
            s3eFileError error = s3eFileGetError();

            LOGGER_ERROR(m_serviceProvider)("MarmaladeOutputStream::flush get error %s [%d]"
				, error_str
                , error
                );

            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
