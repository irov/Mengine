#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
	{
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
	void VideoDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
	{
		m_dataInfo = *static_cast<const VideoCodecDataInfo *>(_dataInfo);
	}
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
}
