#	pragma once

#	include "Codec/VideoDecoder.h"

#	include "ogg/ogg.h"
#	include "theora/theora.h"

namespace Menge
{
	class VideoDecoderTheora
		: public VideoDecoder
	{
	public:
		VideoDecoderTheora();
		~VideoDecoderTheora();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool _prepareData() override;

	public:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;

	public:
		bool _seek( float _timing ) override;
		float _tell() const override;

	public:
		EVideoDecoderReadState readNextFrame( float & _pts ) override;

	public:		
		void setPitch( size_t _pitch ) override;
		
	protected:
		ogg_stream_state m_oggStreamState;
		ogg_sync_state	m_oggSyncState;

		theora_comment m_theoraComment;
		theora_info m_theoraInfo;
		mutable theora_state m_theoraState;

		size_t m_pitch;

	private:
		bool readHeader_();
		size_t read_buffer_data_();
		bool decodeBuffer_( const yuv_buffer & _yuvBuffer, unsigned char * _buffer, size_t _pitch );
		int readFrame_();
	};
}	// namespace Menge
