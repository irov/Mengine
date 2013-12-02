#	include "ImageEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageEncoder::ImageEncoder()
        : m_serviceProvider(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ImageEncoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::initialize( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;

        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<ImageCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr ImageEncoder::getStream() const
    {
        return m_stream;
    }
}
