#include "SoundDecoder.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::setOptions( const CodecOptions * _options )
    {
        if( _options != nullptr )
        {
            m_options = *static_cast<const SoundCodecOptions *>(_options);
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
