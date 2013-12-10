#	include "SoundDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder()
        : m_serviceProvider(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void SoundDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SoundDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }	
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::initialize( const InputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;

        bool status = this->_initialize();

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::setOptions( CodecOptions * _options )
    {
        if( _options != nullptr )
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
    const InputStreamInterfacePtr & SoundDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
