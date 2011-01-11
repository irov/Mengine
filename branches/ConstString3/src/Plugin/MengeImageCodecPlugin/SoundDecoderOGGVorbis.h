/*
 *	SoundDecoderOGGVorbis.h
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "SoundDecoder.h"

#	include "vorbis/vorbisfile.h"


namespace Menge
{
	class SoundDecoderOGGVorbis
		: public SoundDecoder
	{
	public:
		SoundDecoderOGGVorbis( CodecServiceInterface * _service, InputStreamInterface * _stream );
		~SoundDecoderOGGVorbis();

	public:
		bool initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		bool seek( float _timing ) override;
		float timeTell() override;

	protected:
		OggVorbis_File m_oggVorbisFile;
	};
}	// namespace Menge
