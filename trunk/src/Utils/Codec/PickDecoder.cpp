#	include "PickDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickDecoder::PickDecoder()
        : m_serviceProvider(nullptr)
        , m_stream(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PickDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * PickDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::initialize( const InputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;

        bool status = this->_initialize();

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::_initialize()
    {		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != nullptr )
        {
            m_options = *static_cast<PickCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const InputStreamInterfacePtr & PickDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const PickCodecDataInfo * PickDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
