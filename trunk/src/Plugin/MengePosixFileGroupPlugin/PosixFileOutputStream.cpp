#	include "PosixFileOutputStream.h"

#	include "Interface/SDLLayerInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileOutputStream::PosixFileOutputStream()
        : m_serviceProvider(nullptr)
        , m_hFile(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixFileOutputStream::~PosixFileOutputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void PosixFileOutputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileOutputStream::open( const FilePath & _folder, const FilePath& _filename )
	{        
        char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PosixFileOutputStream::open invalid concatenate '%s':'%s'"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

        m_hFile = fopen( filePath, "wb" );

        if( m_hFile == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)("PosixFileOutputStream::open %s:%s get error"
                , _folder.c_str()
                , _filename.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileOutputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
            fflush( m_hFile );
			fclose( m_hFile );
			m_hFile = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileOutputStream::write( const void * _data, size_t _count )
	{
		size_t bytesWritten = fwrite( _data, 1, _count, m_hFile );

        if( bytesWritten != _count )
        {           
            LOGGER_ERROR(m_serviceProvider)("PosixFileOutputStream::write %d:%d get error"
                , bytesWritten
                , _count
                );

            return false;        
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileOutputStream::flush()
	{
		fflush( m_hFile );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
