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
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	public:
		EVideoDecoderReadState readNextFrame( float & _pts ) override;
		bool seek( float _timing ) override;

		float getTiming() const override;
		void setPitch( size_t _pitch ) override;
		
	protected:
		ogg_stream_state m_oggStreamState;
		ogg_sync_state	m_oggSyncState;
		ogg_packet m_oggPacket;

		theora_comment m_theoraComment;
		theora_info m_theoraInfo;
		mutable theora_state m_theoraState;

		yuv_buffer m_yuvBuffer;

		unsigned int m_currentFrame;

		size_t m_pitch;
		size_t m_lastReadBytes;
		bool m_eof;

	private:
		bool readHeader_();
		size_t buffer_data_();
		void decodeBuffer_( unsigned char * _buffer, size_t _pitch );
		int readFrame_();
	};
}	// namespace Menge
