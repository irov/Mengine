#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Core/ConstString.h"

namespace Mengine
{
	struct SoundCodecDataInfo
		: public CodecDataInfo
	{
		SoundCodecDataInfo()
			: size(0)
			, frequency(0)
			, channels(0)
			, bits(0)
			, length(0.f)
		{
		}

		size_t size;
		uint32_t frequency;
		uint32_t channels;
		uint32_t bits;

		float length;
	};

	struct SoundCodecOptions
		: public CodecOptions
	{
	};

	class SoundDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const SoundCodecDataInfo * getCodecDataInfo() const override = 0;
	};

    typedef IntrusivePtr<SoundDecoderInterface> SoundDecoderInterfacePtr;
}
