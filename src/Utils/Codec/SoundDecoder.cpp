#	include "SoundDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder()
        : m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SoundDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream )
    {
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;

        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<SoundCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * SoundDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
