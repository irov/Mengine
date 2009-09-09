#	include "ImageDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoder::ImageDecoder()
		: m_options( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const CodecDataInfo* ImageDecoder::getCodecDataInfo() const 
	{
		if( m_valid == false )
		{
			return NULL;
		}

		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoder::setOptions( unsigned int _options )
	{
		m_options = _options;
	}
}
