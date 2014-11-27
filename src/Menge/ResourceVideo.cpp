#	include "ResourceVideo.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/VideoCodecInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"

#	include "Kernel/ResourceImplement.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo()
		: m_alpha(false)
        , m_noSeek(false)
        , m_frameRate(0)
		, m_duration(0)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::~ResourceVideo()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    float ResourceVideo::getFrameRate() const
    {
        return m_frameRate;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceVideo * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        metadata->swap_File_Path( m_path );
        metadata->swap_File_Codec( m_codecType );
        metadata->swap_File_Converter( m_converterType );        

        metadata->get_File_Alpha( m_alpha );
        metadata->get_File_NoSeek( m_noSeek );
        metadata->get_File_FrameRate( m_frameRate );
		metadata->get_File_Duration( m_duration );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_convert()
    {
		bool result = this->convertDefault_( m_converterType, m_path, m_path, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_isValid() const
    {   
        VideoDecoderInterfacePtr decoder = this->createVideoDecoder();

		bool valid = this->checkValidVideoDecoder_( decoder );

		this->destroyVideoDecoder( decoder );

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::checkValidVideoDecoder_( const VideoDecoderInterfacePtr & _decoder ) const
	{
        if( _decoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceVideo::isValid '%s' can't create decoder '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

		const VideoCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

		
		uint32_t Limit_VideoFrameRate = CONFIG_VALUE(m_serviceProvider, "Limit", "VideoFrameRate", 30U);
		if( dataInfo->fps > Limit_VideoFrameRate )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceVideo.isValid: '%s' path '%s' invalid Frame rate %u more that %u"
				, this->getName().c_str()
				, m_path.c_str()
				, dataInfo->fps
				, Limit_VideoFrameRate
				);

			return false;
		}


   //     if( m_noSeek == false )
   //     {           			
			//for( float pts = 0.f; pts < dataInfo->duration - dataInfo->frameTiming; pts += dataInfo->frameTiming )
			//{
			//	_decoder->seek( pts );
   //         
			//	float current_pts;
			//	EVideoDecoderReadState state = VDRS_FAILURE;
			//	
			//	while( true, true )
			//	{
			//		state = _decoder->readNextFrame( current_pts );

			//		if( state == VDRS_SKIP )
			//		{
			//			continue;
			//		}

			//		break;
			//	}

			//	if( state == VDRS_END_STREAM )
			//	{
			//		break;
			//	}

			//	if( state == VDRS_FAILURE )
			//	{
			//		LOGGER_ERROR(m_serviceProvider)("=============================================================");
			//		LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid read next frame %f:%f (state %d)"
			//			, dataInfo->frameTiming
			//			, current_pts
			//			, state
			//			);

			//		return false;
			//	}

			//	if( fabsf(current_pts - pts) > dataInfo->frameTiming * 2.f )
			//	{
			//		LOGGER_ERROR(m_serviceProvider)("=============================================================");
			//		LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Key Frame %f %f:%f (need one)"
			//			, dataInfo->frameTiming
			//			, pts
			//			, current_pts
			//			);

			//		return false;
			//	}
			//}

            //_decoder->seek( 0.f );
        //}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_release()
	{
		m_videoDecoderCacher.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder() const
    {   
		VideoDecoderInterfacePtr cacheVideoDecoder = m_videoDecoderCacher.findCache();

		if( cacheVideoDecoder != nullptr )
		{
			cacheVideoDecoder->rewind();

			return cacheVideoDecoder;
		}

        const ConstString & category = this->getCategory();

        InputStreamInterfacePtr videoStream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_path, true );

        if( videoStream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't open video file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return nullptr;
        }

        VideoDecoderInterfacePtr videoDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<VideoDecoderInterfacePtr>( m_codecType );

        if( videoDecoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't create video decoder for file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return nullptr;
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

		videoCodecOptions.duration = m_duration;
		videoCodecOptions.fps = m_frameRate;

		videoCodecOptions.mock = CONFIG_VALUE(m_serviceProvider, "Development", "NoVideo", false);
        videoCodecOptions.noSeek = m_noSeek;		

        if( videoDecoder->setOptions( &videoCodecOptions ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't setup options for file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

            return nullptr;
        }

		if( videoDecoder->prepareData( videoStream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't initialize video decoder for file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

			return nullptr;
		}

		m_videoDecoderCacher.addCache( videoDecoder );

        return videoDecoder;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder ) const
	{
		m_videoDecoderCacher.removeCache( _decoder );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_cache()
	{
		m_videoDecoderCacher.lock();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_uncache()
	{
		m_videoDecoderCacher.unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::isAlpha() const
	{
		return m_alpha;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::isNoSkeep() const
    {
        return m_noSeek;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceVideo::getFilePath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceVideo::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
}
