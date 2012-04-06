#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "LogEngine.h"

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
		, m_alpha(false)
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
			BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, m_alpha );
			BIN_CASE_ATTRIBUTE( Protocol::File_Codec, m_codecType );
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
			->openInputFile( category, m_filePath );

		if( m_videoFile == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't open video file '%S'"
				, m_filePath.c_str()
				);

			return false;
		}

		if( m_codecType.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceVideo: you must determine codec for file '%S'"
				, m_filePath.c_str()
				);

			return false;
		}

		m_videoDecoder = CodecEngine::get()
			->createDecoderT<VideoDecoderInterface>( m_codecType, m_videoFile );
				
		if( m_videoDecoder == 0 )
		{
			MENGE_LOG_ERROR( "ResourceVideo: can't create video decoder for file '%S'"
				, m_filePath.c_str()
				);

			m_videoFile->close();

			return false;
		}
		
		VideoCodecOptions videoCodecOptions;
		if( m_alpha == true )
		{
			videoCodecOptions.pixelFormat = Menge::PF_A8R8G8B8;
		}
		else
		{
			videoCodecOptions.pixelFormat = Menge::PF_R8G8B8;
		}
		CodecOptions* codecOptions = dynamic_cast<CodecOptions*>(&videoCodecOptions);
		
		m_videoDecoder->setOptions(  codecOptions );
		
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
			m_videoDecoder->destroy();
			m_videoDecoder = NULL;

			m_videoFile->close();

			m_videoFile = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::isAlpha() const
	{
		return m_alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceVideo::getFilePath() const
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
	void ResourceVideo::rewind( )
	{
		_release();
		_compile();
	}
	//////////////////////////////////////////////////////////////////////////
}
