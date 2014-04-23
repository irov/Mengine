#	pragma once

#	include "Codec/SoundDecoder.h"

#	include "Core/InputStreamBuffer.h"

#	include "vorbis/vorbisfile.h"


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class SoundDecoderOGGVorbis
		: public SoundDecoder
	{
	public:
		SoundDecoderOGGVorbis();
		~SoundDecoderOGGVorbis();

	public:
		bool _initialize() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

		bool seek( float _timing ) override;
		float timeTell() override;

	protected:
		OggVorbis_File m_oggVorbisFile;

		InputStreamBuffer8196 m_streamBuffer;
	};
}	// namespace Menge
