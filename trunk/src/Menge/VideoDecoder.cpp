#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const CodecDataInfo* VideoDecoder::getCodecDataInfo() const 
	{
		if( m_valid == false )
		{
			return NULL;
		}

		return &m_dataInfo;
	}
}
