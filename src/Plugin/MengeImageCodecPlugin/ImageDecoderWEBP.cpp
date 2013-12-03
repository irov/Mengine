#	include "ImageDecoderWEBP.h"
#	include "Interface/FileSystemInterface.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::ImageDecoderWEBP()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::~ImageDecoderWEBP()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<uint8_t> TVectorWEBPBuffer;
    static TVectorWEBPBuffer s_WEBPBuffer;
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_initialize( bool & _version )
	{
		(void)_version;

		size_t dataSize = m_stream->size();
				
		//We must write all data into buffer in initialise because WebP Mux can not determine alpha data in small chunk
		//m_dataBuffer = new uint8_t [m_dataSize];
        s_WEBPBuffer.resize( dataSize );

		uint8_t * buffer = &s_WEBPBuffer[0];
		size_t bufferSize = s_WEBPBuffer.size();

		m_stream->read( buffer, bufferSize );
		
        VP8StatusCode status = WebPGetFeatures( buffer, bufferSize, &m_features );

        if( status != VP8_STATUS_OK )
        {
            return false;
        }
		
        size_t channels;
		if( m_features.has_alpha == 0 )
		{	
			channels = 3;
		}
		else
		{
			channels = 4;
		}

		//fill ImageCodecDataInfo strucuture
		m_dataInfo.width = m_features.width;
		m_dataInfo.height = m_features.height;
		m_dataInfo.depth = 1;
		
		m_dataInfo.mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.channels = channels;
		
        m_options.pitch = m_dataInfo.width * m_dataInfo.channels;

		m_dataInfo.size = s_WEBPBuffer.size();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderWEBP::decode( void * _buffer, size_t _bufferSize )
	{				
        uint8_t * webp_buffer = static_cast<uint8_t *>(_buffer);

#	ifndef MENGE_RENDER_TEXTURE_RGBA
        if( m_features.has_alpha == false )
        {
            if( m_dataInfo.channels == 3 && m_options.channels == 4 )
            {
                if( WebPDecodeBGRInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
                {
                    return 0;
                }

                this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, webp_buffer, m_options.pitch );
            }
            else if( m_dataInfo.channels == 3 && m_options.channels == 3 )
            {
                if( WebPDecodeBGRInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
                {
                    return 0;
                }
            }
        }
        else
        {
            if( m_dataInfo.channels == 4 && m_options.channels == 4 )
            {
                if( WebPDecodeBGRAInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
                {
                    return 0;
                }
            }
        }
#	else
		if( m_features.has_alpha == false )
		{
			if( m_dataInfo.channels == 3 && m_options.channels == 4 )
			{
				if( WebPDecodeRGBInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}

				this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, webp_buffer, m_options.pitch );
			}
			else if( m_dataInfo.channels == 3 && m_options.channels == 3 )
			{
				if( WebPDecodeRGBInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}
			}
		}
		else
		{
			if( m_dataInfo.channels == 4 && m_options.channels == 4 )
			{
				if( WebPDecodeRGBAInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}
			}
		}
#	endif
        
        return _bufferSize;
	}

}	// namespace Menge

