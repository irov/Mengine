#pragma once

#include "Interface/SoundCodecInterface.h"

#include "Codec/Decoder.h"

namespace Mengine
{
	class SoundDecoder
		: public Decoder<SoundDecoderInterface>
	{
	public:
		SoundDecoder();

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const SoundCodecDataInfo * getCodecDataInfo() const override;

	protected:
		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
	};
}
