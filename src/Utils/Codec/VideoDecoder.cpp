#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
        : m_serviceProvider(nullptr)
        , m_stream(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * VideoDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::initialize( const InputStreamInterfacePtr & _stream )
    {        
        m_stream = _stream;

        bool status = this->_initialize();

        return status;
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
    const InputStreamInterfacePtr & VideoDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
