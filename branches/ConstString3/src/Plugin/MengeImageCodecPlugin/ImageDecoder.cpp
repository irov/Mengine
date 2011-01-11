#	include "ImageDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoder::ImageDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream )
		: Decoder(_service,_stream)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const ImageCodecDataInfo * ImageDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoder::setOptions( CodecOptions * _info )
	{
		m_options = *static_cast<ImageCodecOptions*>(_info);

		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoder::_invalidate()
	{
		//Empty
	}
}
