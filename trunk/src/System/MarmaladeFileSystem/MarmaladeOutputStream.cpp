#	include "MarmaladeOutputStream.h"

#	include "Interface/LogSystemInterface.h"

#   include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeOutputStream::MarmaladeOutputStream( ServiceProviderInterface * _serviceProvider )		
        : m_serviceProvider(_serviceProvider)
        , m_hFile(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeOutputStream::~MarmaladeOutputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeOutputStream::open( const FilePath& _filePath )
	{        
        m_hFile = s3eFileOpen( _filePath.c_str(), "wb" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeOutputStream::destroy()
	{
		if( m_hFile != NULL )
		{
			s3eFileClose( m_hFile );
			m_hFile = NULL;
		}

        delete this;
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
