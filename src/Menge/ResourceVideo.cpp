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
		, m_stream( 0 )
		, m_buffer( 0 )
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
		DataStreamInterface* stream = Holder<FileEngine>::hostage()->openFile( m_filepath );
		m_stream = CodecManager::getCodec( "theora" );
		ImageCodec::ImageData imageData;
		bool res = m_stream->start( stream, static_cast<CodecInterface::CodecData*>( &imageData ) );
		if( res == false )
		{
			MENGE_LOG( MENGE_TEXT("Error: failed to compile theora video") );
			return false;
		}

		m_frameSize.x = imageData.width;
		m_frameSize.y = imageData.height;

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
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::sync( float _timing )
	{
		while( m_stream->sync( _timing ) < 0 )	// if we are not up to date read frame
		{
			m_stream->read( m_buffer, m_bufferSize );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* ResourceVideo::getRGBData()
	{
		return m_buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceVideo::getFrameSize()
	{
		return m_frameSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge