/*
 *	VideoDecoderOGGTheora.h
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once


#	include "VideoDecoder.h"

#	include "ogg/ogg.h"
#	include "theora/theora.h"

namespace Menge
{
	class VideoDecoderOGGTheora
		: public VideoDecoder
	{
	public:
		VideoDecoderOGGTheora( CodecServiceInterface * _service, InputStreamInterface * _stream );
		~VideoDecoderOGGTheora();

	public:
		bool initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	public:
		bool eof() override;

		int sync( float _timing ) override;
		bool seek( float _timing ) override;

		//Lookup tables for ColorSpace conversions
		static void createCoefTables_();

	protected:
		ogg_stream_state m_oggStreamState;
		ogg_sync_state	m_oggSyncState;
		ogg_page m_oggPage;
		ogg_packet m_oggPacket;

		theora_comment m_theoraComment;
		theora_info m_theoraInfo;
		theora_state m_theoraState;

		yuv_buffer m_yuvBuffer;

		unsigned int m_currentFrame;

		int m_lastReadBytes;
		bool m_eof;

		static signed int ms_YTable[ 256 ];
		static signed int ms_BUTable[ 256 ];
		static signed int ms_GUTable[ 256 ];
		static signed int ms_GVTable[ 256 ];
		static signed int ms_RVTable[ 256 ];

	private:
		bool readHeader_();
		void clear_();
		std::streamsize buffer_data_();
		void decodeBuffer_( unsigned char* _buffer, int _pitch );
		int readFrame_();
	};
}	// namespace Menge
