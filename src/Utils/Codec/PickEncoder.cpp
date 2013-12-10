#	include "PickEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickEncoder::PickEncoder()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PickEncoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * PickEncoder::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
    bool PickEncoder::initialize( const OutputStreamInterfacePtr & _stream )
    {
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
    OutputStreamInterfacePtr PickEncoder::getStream() const
    {
        return m_stream;
    }
}
