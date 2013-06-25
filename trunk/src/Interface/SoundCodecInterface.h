#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Utils/Core/ConstString.h"

namespace Menge
{
	struct SoundCodecDataInfo
		: public CodecDataInfo
	{
		int frequency;
		int channels;
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
		virtual bool seek( float _timing ) = 0;
		virtual float timeTell() = 0;

		virtual const SoundCodecDataInfo * getCodecDataInfo() const override = 0;
	};

    typedef IntrusivePtr<SoundDecoderInterface> SoundDecoderInterfacePtr;
}
