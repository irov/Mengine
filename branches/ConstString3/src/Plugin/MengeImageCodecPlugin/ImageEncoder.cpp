#	include "ImageEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageEncoder::ImageEncoder( CodecServiceInterface * _service, OutputStreamInterface * _stream )
		: Encoder(_service, _stream)
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
