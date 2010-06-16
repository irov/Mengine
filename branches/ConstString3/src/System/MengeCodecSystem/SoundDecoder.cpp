#	include "SoundDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const 
	{
		if( m_valid == false )
		{
			return NULL;
		}

		return &m_dataInfo;
	}
}
