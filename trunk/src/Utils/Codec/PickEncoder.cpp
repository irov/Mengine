#	include "PickEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickEncoder::PickEncoder()
        : m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::initialize( ServiceProviderInterface * _serviceProvider, OutputStreamInterface * _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;

        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<PickCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterface * PickEncoder::getStream() const
    {
        return m_stream;
    }
}
