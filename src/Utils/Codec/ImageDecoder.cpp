#	include "ImageDecoder.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoder::ImageDecoder()
		: m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ImageDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream )
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
    InputStreamInterface * ImageDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const ImageCodecDataInfo * ImageDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
