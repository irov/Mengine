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
		this->cleanup_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderWEBP::cleanup_()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<uint8_t> TVectorWEBPBuffer;
    static TVectorWEBPBuffer s_WEBPBuffer;
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderWEBP::_initialize()
	{
		size_t dataSize = m_stream->size();
				
		//We must write all data into buffer in initialise because WebP Mux can not determine alpha data in small chunk
		//m_dataBuffer = new uint8_t [m_dataSize];
        s_WEBPBuffer.resize( dataSize );
		m_stream->read( &s_WEBPBuffer[0], s_WEBPBuffer.size() );
		
        VP8StatusCode status = WebPGetFeatures( &s_WEBPBuffer[0], s_WEBPBuffer.size(), &m_features );

        if( status != VP8_STATUS_OK )
        {
            return false;
        }
		
        size_t channels;
		if( m_features.has_alpha == false )
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
        if( m_features.has_alpha == false )
        {
            if( WebPDecodeBGRInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), (uint8_t*)_buffer, _bufferSize, m_options.pitch ) == nullptr )
            {
                return 0;
            }
        }
        else
        {
            if( WebPDecodeBGRAInto( &s_WEBPBuffer[0], s_WEBPBuffer.size(), (uint8_t*)_buffer, _bufferSize, m_options.pitch ) == nullptr )
            {
                return 0;
            }
        }
        
        return _bufferSize;
	}

}	// namespace Menge

