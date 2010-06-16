
#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{

	struct SoundCodecDataInfo
		: public CodecDataInfo
	{
		int frequency;
		int channels;
		float time_total_secs;
	};

	class SoundDecoderInterface
		: virtual public DecoderInterface
	{
	public:
		virtual bool seek( float _timing ) = 0;
		virtual float timeTell() = 0;

		virtual const SoundCodecDataInfo * getCodecDataInfo() const override = 0;
	};
}	// namespace Menge
