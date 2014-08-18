#	pragma once

#	include "Codec/SoundDecoder.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class SoundDecoderWAV
		: public SoundDecoder
	{
	public:
		SoundDecoderWAV();
		~SoundDecoderWAV();

	public:
		bool _prepareData() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

		bool seek( float _timing ) override;
		float timeTell() override;

	protected:		
		bool findChunkData_( size_t & _size );

	protected:
		size_t m_chunkDataPos;
	};
}	// namespace Menge
