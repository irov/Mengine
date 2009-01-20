/*
 *	SoundDecoderOGGVorbis.h
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/SoundCodecInterface.h"
#	include "DecoderDeclare.h"

struct OggVorbis_File;

namespace Menge
{
	class SoundDecoderOGGVorbis
		: public SoundDecoderInterface
	{
		DECODER_DECLARE( SoundDecoderOGGVorbis );

	public:
		SoundDecoderOGGVorbis( DataStreamInterface* _stream, const String& _type );
		~SoundDecoderOGGVorbis();

	public:
		void destructor() override;
		void release() override;
		const String& getType() const override;

		DataStreamInterface* getStream() override;
		const CodecDataInfo* getCodecDataInfo() const override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		bool seek( float _timing ) override;
		float timeTell() override;

	private:
		bool m_valid;
		DataStreamInterface* m_stream;
		SoundCodecDataInfo m_dataInfo;

		bool readHeader_();
		OggVorbis_File* m_oggVorbisFile;
	};
}	// namespace Menge
