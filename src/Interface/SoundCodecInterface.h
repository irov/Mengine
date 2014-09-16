#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Utils/Core/ConstString.h"

namespace Menge
{
	struct SoundCodecDataInfo
		: public CodecDataInfo
	{
		uint32_t frequency;
		uint32_t channels;
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
		virtual float tell() = 0;

		virtual const SoundCodecDataInfo * getCodecDataInfo() const override = 0;
	};

    typedef stdex::intrusive_ptr<SoundDecoderInterface> SoundDecoderInterfacePtr;
}
