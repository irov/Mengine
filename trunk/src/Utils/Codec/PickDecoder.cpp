#	include "PickDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickDecoder::PickDecoder()
	{
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
	void PickDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
	{
		m_dataInfo = *static_cast<const PickCodecDataInfo *>(_dataInfo);
	}
	//////////////////////////////////////////////////////////////////////////
	const PickCodecDataInfo * PickDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
