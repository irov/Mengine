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
		SoundDecoder( FileInputInterface * _stream );

	public:
		const SoundCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( CodecOptions * _options ) override;

	protected:
		virtual void _invalidate();

	protected:
		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
	};
}
