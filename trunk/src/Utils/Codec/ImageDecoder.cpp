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
	void ImageDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ImageDecoder::getServiceProvider() const
	{
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::initialize( const InputStreamInterfacePtr & _stream, bool & _version )
    {
        m_stream = _stream;

		_version = true;
        
        bool result = this->_initialize( _version );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != nullptr )
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
    bool ImageDecoder::_initialize( bool & _version )
    {
		(void)_version;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const InputStreamInterfacePtr & ImageDecoder::getStream() const
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

        for( size_t h = _height; h != 0; --h )
        {
            size_t hp = (h - 1) * _pitch;

            for( size_t w = _width; w != 0; --w )
            {
                sweezle_buffer[hp + (w - 1) * 4 + 3] = sweezle_buffer[hp + (w - 1)];
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::sweezleAlpha3( size_t _width, size_t _height, void * _buffer, size_t _pitch )
    {
        unsigned char * sweezle_buffer = static_cast<unsigned char *>(_buffer);

        for( size_t h = _height; h != 0; --h )
        {
            size_t hp = (h - 1) * _pitch;

            for( size_t w = _width; w != 0; --w )
            {
                unsigned char * sweezle_buffer_rgba = sweezle_buffer + hp + (w - 1) * 4;
                unsigned char * sweezle_buffer_rgb = sweezle_buffer + hp + (w - 1) * 3;

                sweezle_buffer_rgba[0] = sweezle_buffer_rgb[0];
                sweezle_buffer_rgba[1] = sweezle_buffer_rgb[1];
                sweezle_buffer_rgba[2] = sweezle_buffer_rgb[2];
                sweezle_buffer_rgba[3] = 255;
            }
        }
    }

}
