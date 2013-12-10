/*
 *	SoundDecoderOGGVorbis.h
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

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
		bool _initialize() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

		bool seek( float _timing ) override;
		float timeTell() override;

	protected:
		OggVorbis_File m_oggVorbisFile;
	};
}	// namespace Menge
