#	include "SoundDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoder::SoundDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream )
		: Decoder(_service, _stream)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoder::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<SoundCodecOptions*>(_options);

		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoder::_invalidate()
	{
		//Empty
	}

}
