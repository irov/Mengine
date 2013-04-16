#	include "PickDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickDecoder::PickDecoder()
        : m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * PickDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;

        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
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
    InputStreamInterface * PickDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const PickCodecDataInfo * PickDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
