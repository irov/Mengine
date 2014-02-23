#	include "SoundDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder()
	{
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
	void SoundDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
	{
		m_dataInfo = *static_cast<const SoundCodecDataInfo *>(_dataInfo);
	}
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
