#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "Codec.h"

#	include "Interface/VideoCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_bufferSize( 0 )
		, m_frameSize( 0.0f, 0.0f )
		, m_videoDecoder( NULL )
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
			XML_CASE_ATTRIBUTE_NODE( "File", "Path", m_filepath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		if( m_filepath.empty() == true )
		{
			return false;
		}

		m_videoDecoder = Holder<DecoderManager>::hostage()
			->createDecoderT<VideoDecoderInterface>( m_params.category + m_filepath, "Video" );

		if( m_videoDecoder == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't create video decoder for file \"%s\""
						, m_filepath.c_str() );
			return false;
		}

		const VideoCodecDataInfo * dataInfo = static_cast<const VideoCodecDataInfo*>( m_videoDecoder->getCodecDataInfo() );
		m_frameSize.x = dataInfo->frame_width;
		m_frameSize.y = dataInfo->frame_height;

		m_bufferSize =  m_frameSize.x * m_frameSize.y * 4;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_release()
	{
		if( m_videoDecoder != NULL )
		{
			Holder<DecoderManager>::hostage()
				->releaseDecoder( m_videoDecoder );

			m_videoDecoder = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceVideo::getFilename() const
	{
		return m_filepath;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::sync( float _timing )
	{
		if( m_videoDecoder->sync( _timing ) < 0 )	// if we are not up to date read frame
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::getRGBData( unsigned char* _buffer, int _pitch )
	{
		m_videoDecoder->decode( _buffer, _pitch );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceVideo::getFrameSize()
	{
		return m_frameSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::eof()
	{
		return m_videoDecoder->eof();
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
}
