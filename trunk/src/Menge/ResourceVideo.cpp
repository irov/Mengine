#	include "ResourceVideo.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/VideoCodecInterface.h"
#   include "Interface/StringizeInterface.h"

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
        , m_frameRate(0.f)
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
        metadata->swap_File_Converter( m_converter );        

        metadata->get_File_Alpha( m_alpha );
        metadata->get_File_NoSeek( m_noSeek );
        metadata->get_File_FrameRate( m_frameRate );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_convert()
    {
		bool result = this->convertDefault_( m_converter, m_path, m_path, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_isValid() const
    {   
        VideoDecoderInterfacePtr decoder = this->createVideoDecoder();

        if( decoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceVideo::isValid '%s' can't create decoder '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        if( m_noSeek == false )
        {
            const VideoCodecDataInfo * dataInfo = decoder->getCodecDataInfo();
			
			for( float pts = 0.f; pts < 200.f; pts += dataInfo->frameTiming )
			{
				decoder->seek( pts );
            
				float current_pts;
				EVideoDecoderReadState state = VDRS_FAILURE;
				
				while( true, true )
				{
					state = decoder->readNextFrame( current_pts );

					if( state == VDRS_SKIP )
					{
						continue;
					}

					break;
				}

				if( state == VDRS_END_STREAM )
				{
					break;
				}

				if( state == VDRS_FAILURE )
				{
					LOGGER_ERROR(m_serviceProvider)("=============================================================");
					LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid read next frame %f:%f (state %d)"
						, dataInfo->frameTiming
						, current_pts
						, state
						);

					return false;
				}

				if( fabsf(current_pts - pts) > dataInfo->frameTiming * 2.f )
				{
					LOGGER_ERROR(m_serviceProvider)("=============================================================");
					LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Key Frame %f %f:%f (need one)"
						, dataInfo->frameTiming
						, pts
						, current_pts
						);

					return false;
				}
			}

            decoder->seek( 0.f );
        }

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
	}
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder() const
    {        
        const ConstString & category = this->getCategory();

        InputStreamInterfacePtr videoStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( category, m_path );

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

		if( videoDecoder->initialize( videoStream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't initialize video decoder for file '%s'"
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

        videoCodecOptions.noSeek = m_noSeek;

        if( videoDecoder->setOptions( &videoCodecOptions ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceVideo::createVideDecoder '%s' can't setup options for file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

            return nullptr;
        }

        return videoDecoder;
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
