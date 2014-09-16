#	pragma once

#	include "Codec/SoundDecoder.h"

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
		bool _prepareData() override;
		bool _rewind() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
		
	public:
		bool seek( float _timing ) override;
		float tell() override;

	protected:
		OggVorbis_File m_oggVorbisFile;

		volatile bool m_process;
	};
}	// namespace Menge
