
#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Utils/Core/ConstString.h"
namespace Menge
{
    const unsigned long fixed_sound_buffer_size = 32768; // 32 KB buffers

	struct SoundCodecDataInfo
		: public CodecDataInfo
	{
		int frequency;
		int channels;
		float time_total_secs;
	};

	struct SoundCodecOptions
		: public CodecOptions
	{
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
