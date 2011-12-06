#	include "ImageDecoder.h"
#	include "ImageDecoderCombinerRGBAndAlpha.h"
#	include "Utils/Logger/Logger.h"

namespace Menge
{
/*
Combine alpha and rgb decoders  and  realize ImageDecoderInterface 
*/

	ImageDecoderCombinerRGBAndAlpha::ImageDecoderCombinerRGBAndAlpha( ImageDecoderInterface * _decoderRGB, ImageDecoderInterface * _decoderAlpha, LogSystemInterface * _logSystem )
		: m_decoderAlpha(_decoderAlpha)
		, m_decoderRGB(_decoderRGB)
		, m_logSystem(_logSystem)
		, m_rowStride(0)
		, m_bufferRowStride(0)
	{
		//delete it when you perform creating instances of this class  in   CodecEngine::get()->createDecoderT<ImageDecoderInterface> method
		 initialize();
	}
	//////////////////////////////////////////////////////////////////////////			
	ImageDecoderCombinerRGBAndAlpha::~ImageDecoderCombinerRGBAndAlpha()
	{
	}
	//////////////////////////////////////////////////////////////////////////	
	bool ImageDecoderCombinerRGBAndAlpha::initialize()
	{
		const ImageCodecDataInfo* infoRGB = m_decoderRGB->getCodecDataInfo();
		
		m_dataInfo.format = PF_A8R8G8B8;
		m_dataInfo.depth = 1;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.width = infoRGB->width;
		m_dataInfo.height = infoRGB->height;
		m_dataInfo.size = m_dataInfo.width * m_dataInfo.height * 4;
		m_rowStride =  m_dataInfo.width * 4;
		m_bufferRowStride = m_dataInfo.width * 4;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderCombinerRGBAndAlpha::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
			
		//m_bufferRowStride = m_dataInfo.width * 4;

		if( ((m_bufferRowStride < m_rowStride) || ((_bufferSize % m_bufferRowStride) != 0)) )
		{
			LOGGER_ERROR(m_logSystem)( "ImageDecoderCombinerRGBAndAlpha::decode error, invalid buffer pitch or size" );
			return 0;
		}

		if( (_bufferSize < ( m_dataInfo.height * m_rowStride )) )
		{
			LOGGER_ERROR(m_logSystem)( "ImageDecoderCombinerRGBAndAlpha::decode error - invalid buffer size. Can decode only whole image at once" );
			return 0;
		}
				
		//get RGB data
		ImageCodecOptions optionsRGB;
		optionsRGB.flags = DF_COUNT_ALPHA | DF_CUSTOM_PITCH;
		optionsRGB.pitch = m_options.pitch;
		m_decoderRGB->setOptions( &optionsRGB );

		if( m_decoderRGB->decode( _buffer, _bufferSize ) == 0 )
		{
			return 0;
		}
		
		const ImageCodecDataInfo* alphaDataInfo = m_decoderAlpha->getCodecDataInfo();
		ImageCodecOptions optionsAlpha;
		optionsAlpha.flags = DF_READ_ALPHA_ONLY | DF_CUSTOM_PITCH;
		optionsAlpha.pitch = alphaDataInfo->width;
		m_decoderAlpha->setOptions( &optionsAlpha );

		
		
		// alpha must 1 channel 8 bit depth
		if(	alphaDataInfo == NULL 
			|| m_dataInfo.width != alphaDataInfo->width 
			|| m_dataInfo.height != alphaDataInfo->height )
		{
			LOGGER_ERROR(m_logSystem)( "ImageDecoderCombinerRGBAndAlpha::decode error while decoding image. Can't find png data" );
			LOGGER_ERROR(m_logSystem)( "format %i, width (%i,%i) height (%i,%i) pointer %p"
				, (int)alphaDataInfo->format
				, m_dataInfo.width
				, alphaDataInfo->width
				, m_dataInfo.height
				, alphaDataInfo->height
				, alphaDataInfo
				);

			return 0;
		}
		
		size_t AlphaBufferSize = alphaDataInfo->width * alphaDataInfo->height;
		unsigned char* alphaBuffer = new unsigned char[ AlphaBufferSize ];
		unsigned char* alphaBufferCursor = alphaBuffer;
		
		if( m_decoderAlpha->decode( alphaBuffer, AlphaBufferSize ) == 0 )
		{
			return 0;
		}
		
		for( std::size_t i = 0; i < m_dataInfo.height; ++i )
		{
			for( std::size_t j = 0; j < m_dataInfo.width; ++j )
			{
				_buffer[j*4+3] = alphaBufferCursor[j];
			}
			_buffer += m_bufferRowStride;
			alphaBufferCursor += alphaDataInfo->width;
		}
		
		alphaBufferCursor = NULL;
		delete[] alphaBuffer;
		
		return m_dataInfo.height * m_rowStride;
	}
	//////////////////////////////////////////////////////////////////////////
	const ImageCodecDataInfo * ImageDecoderCombinerRGBAndAlpha::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderCombinerRGBAndAlpha::_invalidate()
	{
		m_bufferRowStride = m_options.pitch;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderCombinerRGBAndAlpha::setOptions( CodecOptions * _info )
	{
		m_options = *static_cast<ImageCodecOptions*>(_info);

		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface * ImageDecoderCombinerRGBAndAlpha::getStream( ) const
	{
		return m_decoderAlpha->getStream();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderCombinerRGBAndAlpha::destroy( ) 
	{
		delete this; 
		//m_decoderAlpha->destroy();
		//m_decoderRGB->destroy();
	}
	//////////////////////////////////////////////////////////////////////////
}	
// namespace Menge
