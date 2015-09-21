#	pragma once

#	include "Codec/SoundDecoder.h"

#	include "vorbis/vorbisfile.h"

#	include "stdex/thread_guard.h"

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
		void _finalize() override;

	public:
		bool _prepareData() override;
		bool _rewind() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
		
	public:
		bool seek( float _timing ) override;
		float tell() override;

	protected:
		OggVorbis_File m_oggVorbisFile;

		bool m_oggVorbisFileInitialize;

		STDEX_THREAD_GUARD_INIT;
	};
}	// namespace Menge
