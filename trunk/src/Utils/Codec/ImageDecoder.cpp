#	include "ImageDecoder.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoder::ImageDecoder()
		: m_serviceProvider(nullptr)
        , m_stream(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ImageDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::initialize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;
        
        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<ImageCodecOptions *>(_options);
        }

        if( m_options.pitch == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoder::setOptions pitch == 0"
                );

            return false;
        }

        if( m_options.channels == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoder::setOptions channels == 0"
                );

            return false;
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr ImageDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const ImageCodecDataInfo * ImageDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::sweezleAlpha1( size_t _width, size_t _height, void * _buffer, size_t _pitch )
    {
        unsigned char * sweezle_buffer = static_cast<unsigned char *>(_buffer);

        for( size_t h = _height - 1; h != -1; --h )
        {
            size_t hp = h * _pitch;

            for( size_t w = _width - 1; w != -1; --w )
            {
                sweezle_buffer[hp + w * 4 + 3] = sweezle_buffer[hp + w];
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::sweezleAlpha3( size_t _width, size_t _height, void * _buffer, size_t _pitch )
    {
        unsigned char * sweezle_buffer = static_cast<unsigned char *>(_buffer);

        for( size_t h = _height - 1; h != -1; --h )
        {
            size_t hp = h * _pitch;

            for( size_t w = _width - 1; w != -1; --w )
            {
                sweezle_buffer[hp + w * 4 + 0] = sweezle_buffer[hp + w * 3 + 0];
                sweezle_buffer[hp + w * 4 + 1] = sweezle_buffer[hp + w * 3 + 1];
                sweezle_buffer[hp + w * 4 + 2] = sweezle_buffer[hp + w * 3 + 2];
                sweezle_buffer[hp + w * 4 + 3] = 255;
            }
        }
    }

}
