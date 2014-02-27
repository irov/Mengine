#	include "ImageDecoderWEBP.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::ImageDecoderWEBP()
		: m_bufferId(0)
		, m_memory(nullptr)
		, m_memorySize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderWEBP::~ImageDecoderWEBP()
	{
		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_bufferId );

		m_bufferId = 0;
		m_memory = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_initialize()
	{
		size_t bufferSize = m_stream->size();
				
		void * memory = nullptr;
		m_bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( bufferSize, &memory );

		m_memory = static_cast<uint8_t *>(memory);
		m_memorySize = bufferSize;

		m_stream->read( m_memory, m_memorySize );
		
        VP8StatusCode status = WebPGetFeatures( m_memory, m_memorySize, &m_features );

        if( status != VP8_STATUS_OK )
        {
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderWEBP::_initialize invalid WebPGetFeatures %d"
				, status
				);

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

		m_dataInfo.size = bufferSize;

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
                if( WebPDecodeBGRInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
                {
                    return 0;
                }

                this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, webp_buffer, m_options.pitch );
            }
            else if( m_dataInfo.channels == 3 && m_options.channels == 3 )
            {
                if( WebPDecodeBGRInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
                {
                    return 0;
                }
            }
        }
        else
        {
            if( m_dataInfo.channels == 4 && m_options.channels == 4 )
            {
                if( WebPDecodeBGRAInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
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
				if( WebPDecodeRGBInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}

				this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, webp_buffer, m_options.pitch );
			}
			else if( m_dataInfo.channels == 3 && m_options.channels == 3 )
			{
				if( WebPDecodeRGBInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}
			}
		}
		else
		{
			if( m_dataInfo.channels == 4 && m_options.channels == 4 )
			{
				if( WebPDecodeRGBAInto( m_memory, m_memorySize, webp_buffer, _bufferSize, m_options.pitch ) == nullptr )
				{
					return 0;
				}
			}
		}
#	endif
        
        return _bufferSize;
	}

}	// namespace Menge

