#	include "ImageEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageEncoder::ImageEncoder( FileOutputInterface * _stream )
		: Encoder(_stream)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageEncoder::setOptions( CodecOptions * _info )
	{
		m_options = *static_cast<ImageCodecOptions*>(_info);
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageEncoder::_invalidate()
	{
		//Empty
	}
}
