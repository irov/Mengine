#	include "MarmaladeOutputStream.h"

#	include "Interface/LogSystemInterface.h"

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
	bool MarmaladeOutputStream::open( const FilePath& _filePath )
	{        
        m_hFile = s3eFileOpen( _filePath.c_str(), "wb" );

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
