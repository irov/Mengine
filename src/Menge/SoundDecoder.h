#	pragma once

#	include "Decoder.h"

#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	class SoundDecoder
		: public Decoder
		, public SoundDecoderInterface
	{
	public:
		SoundDecoder();

	public:
		const CodecDataInfo* getCodecDataInfo() const override;

	protected:
		SoundCodecDataInfo m_dataInfo;
	};
}