#	include "MarmaladeOutputStream.h"

#	include "Interface/MarmaladeLayerInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeOutputStream::MarmaladeOutputStream()		
        : m_serviceProvider(nullptr)
        , m_hFile(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeOutputStream::~MarmaladeOutputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeOutputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeOutputStream::open( const FilePath & _folder, const FilePath& _filename )
	{        
        char filePath[MAX_PATH];
        if( MARMALADELAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _filename, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeOutputStream::open invalid concatenate '%s':'%s'"
                , _folder
                , _filename
                );

            return false;
        }

        m_hFile = s3eFileOpen( filePath, "wb" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeOutputStream::_destroy()
	{
		if( m_hFile != nullptr )
		{
			s3eFileClose( m_hFile );
			m_hFile = nullptr;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeOutputStream::write( const void * _data, size_t _count )
	{
        uint32 bytesWritten = s3eFileWrite( _data, _count, 1, m_hFile );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeOutputStream::flush()
	{
		s3eFileFlush( m_hFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
