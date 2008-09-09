#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "Codec.h"
#	include "ImageCodec.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_filestream( 0 )
		, m_stream( 0 )
		, m_bufferSize( 0 )
		, m_frameSize( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::~ResourceVideo()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("FilePath"), MENGE_TEXT("Value"), m_filepath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		if( m_filepath == MENGE_TEXT("") )
		{
			return false;
		}
		m_filestream = Holder<FileEngine>::hostage()->openFile( m_params.category + m_filepath );
		if( m_filestream == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Resource Video: file not found") );
			return false;
		}
		m_stream = CodecManager::getCodec( "theora" );
		ImageCodec::ImageData imageData;
		bool res = m_stream->start( m_filestream, static_cast<CodecInterface::CodecData*>( &imageData ) );
		if( res == false )
		{
			MENGE_LOG( MENGE_TEXT("Error: failed to compile theora video") );
			return false;
		}

		m_frameSize.x = imageData.width;
		m_frameSize.y = imageData.height;

		m_bufferSize =  m_frameSize.x * m_frameSize.y * 4;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_release()
	{
		if( m_stream != 0 )
		{
			m_stream->finish();
			m_stream = 0;
		}
		if( m_filestream != 0 )
		{
			Holder<FileEngine>::hostage()
				->closeStream( m_filestream );
			m_filestream = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::sync( float _timing )
	{
		if( m_stream->sync( _timing ) < 0 )	// if we are not up to date read frame
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::getRGBData( unsigned char* _buffer )
	{
		m_stream->read( _buffer, m_bufferSize );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceVideo::getFrameSize()
	{
		return m_frameSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::eof()
	{
		return m_stream->eof();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::seek( float _timing )
	{
		//m_stream->seek( _timing );
		if( _timing == 0.0f )
		{
			//m_stream->finish();
			//m_stream->start( m_filestream, 
			_release();
			_compile();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge