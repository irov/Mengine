#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

#	include "Interface/VideoCodecInterface.h"

#	include "FileEngine.h"
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
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_filePath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		if( m_filePath.empty() == true )
		{
			return false;
		}

		const ConstString & category = this->getCategory();

		m_videoFile = FileEngine::get()
			->openInputFile( category, m_filePath.str() );

		if( m_videoFile == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't open video file '%s'"
				, m_filePath.c_str()
				);
		}

		m_videoDecoder = CodecEngine::get()
			->createDecoderT<VideoDecoderInterface>( Consts::get()->c_Video, m_videoFile );

		if( m_videoDecoder == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't create video decoder for file '%s'"
				, m_filePath.c_str()
				);

			FileEngine::get()
				->closeInputFile( m_videoFile );

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

			FileEngine::get()
				->closeInputFile( m_videoFile );

			m_videoFile = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceVideo::getFilePath() const
	{
		return m_filePath;
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
