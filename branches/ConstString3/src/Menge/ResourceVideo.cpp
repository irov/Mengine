#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

#	include "Interface/VideoCodecInterface.h"

#	include "CodecEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo()
		: m_bufferSize(0)
		, m_frameSize(0.0f, 0.0f)
		, m_videoDecoder(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::~ResourceVideo()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_filepath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		if( m_filepath.empty() == true )
		{
			return false;
		}

		const ConstString & category = this->getCategory();

		m_videoDecoder = CodecEngine::get()
			->createDecoderT<VideoDecoderInterface>( category, m_filepath, Consts::get()->c_Video );

		if( m_videoDecoder == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't create video decoder for file '%s'"
				, m_filepath.c_str()
				);

			return false;
		}

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();
		m_frameSize.x = dataInfo->frame_width;
		m_frameSize.y = dataInfo->frame_height;

		m_bufferSize =  static_cast<std::streamsize>(m_frameSize.x * m_frameSize.y * 4.f);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_release()
	{
		if( m_videoDecoder != NULL )
		{
			CodecEngine::get()
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
	const mt::vec2f& ResourceVideo::getFrameSize() const
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
