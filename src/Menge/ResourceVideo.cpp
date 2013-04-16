#	include "ResourceVideo.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/VideoCodecInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Kernel/ResourceImplement.h"

#	include "Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo()
		: m_alpha(false)
        , m_noSeek(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::~ResourceVideo()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceVideo * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        metadata->swap_File_Path( m_path );
        metadata->swap_File_Codec( m_codec );
        metadata->swap_File_Converter( m_converter );        

        metadata->get_File_Alpha( m_alpha );
        metadata->get_File_NoSeek( m_noSeek );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_convert()
    {
        if( m_path.empty() == true )
        {
            return false;
        }

        //perform convertation if we need
        if ( m_converter.empty() == false )
        {
            if( this->convert_() == false )
            {
                return false;
            }
        }

        if( m_codec.empty() == true )
        {
            m_codec = this->getCodec_( m_path );
        }

        if( m_codec.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceVideo::_convert %s you must determine codec for file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_isValid() const
    {   
        VideoDecoderInterface * decoder = this->createVideoDecoder();

        if( decoder == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceVideo::isValid '%s' can't create decoder '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        decoder->destroy();

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
    bool ResourceVideo::convert_()
    {
        const ConstString & category = this->getCategory();

        if( CONVERTER_SERVICE(m_serviceProvider)
            ->convert( m_converter, category, m_path, m_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceVideo::convert: '%s' can't convert '%s':'%s'"
                , this->getName().c_str() 
                , m_path.c_str()
                , m_converter.c_str()
                );

            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterface * ResourceVideo::createVideoDecoder() const
    {        
        const ConstString & category = this->getCategory();

        InputStreamInterface * videoStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( category, m_path );

        if( videoStream == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceVideo::createVideDecoder '%s' can't open video file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return NULL;
        }

        VideoDecoderInterface * videoDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<VideoDecoderInterface>( m_codec, videoStream );

        if( videoDecoder == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceVideo::createVideDecoder '%s' can't create video decoder for file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            videoStream->destroy();

            return NULL;
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
            videoDecoder->destroy();

            return NULL;
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
		return m_codec;
	}
	//////////////////////////////////////////////////////////////////////////
}
