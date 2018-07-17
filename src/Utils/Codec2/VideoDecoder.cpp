#include "VideoDecoder.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    VideoDecoder::~VideoDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoder::setOptions( const CodecOptions * _options )
    {
        if( _options != nullptr )
        {
            m_options = *static_cast<const VideoCodecOptions *>(_options);
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
