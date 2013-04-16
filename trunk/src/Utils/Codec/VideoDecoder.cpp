#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
        : m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * VideoDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;
                
        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<VideoCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * VideoDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
