#	include "ImageDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoder::ImageDecoder( FileInputInterface * _stream )
		: Decoder(_stream)
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
