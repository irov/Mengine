#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const 
	{
		if( m_valid == false )
		{
			return NULL;
		}

		return &m_dataInfo;
	}
}
