/*
 *	VideoDecoderOGGTheora.h
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/VideoCodecInterface.h"
#	include "ogg/ogg.h"
#	include "theora/theora.h"

namespace Menge
{
	class VideoDecoderOGGTheora
		: public VideoDecoderInterface
	{
		MENGE_DECLARE_DECODER( VideoDecoderOGGTheora );

	public:
		VideoDecoderOGGTheora( DataStreamInterface* _stream, const String& _type );
		~VideoDecoderOGGTheora();

	public:
		void destructor() override;
		virtual void release() override;
		virtual const String& getType() const override;

		DataStreamInterface* getStream() override;
		const CodecDataInfo* getCodecDataInfo() const override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		int sync( float _timing ) override;
		bool seek( float _timing ) override;

		//Lookup tables for ColorSpace conversions
		static void createCoefTables_();

	private:
		bool m_valid;
		DataStreamInterface* m_stream;
		VideoCodecDataInfo m_dataInfo;
		bool readHeader_();

		ogg_stream_state m_oggStreamState;
		ogg_sync_state	m_oggSyncState;
		ogg_page m_oggPage;
		ogg_packet m_oggPacket;

		theora_comment m_theoraComment;
		theora_info m_theoraInfo;
		theora_state m_theoraState;

		yuv_buffer m_yuvBuffer;

		unsigned int m_currentFrame;

		void clear_();
		std::streamsize buffer_data_();
		void decodeBuffer_( unsigned char* _buffer, int _pitch );
		int readFrame_();
		int m_lastReadBytes;


		static signed int ms_YTable[ 256 ];
		static signed int ms_BUTable[ 256 ];
		static signed int ms_GUTable[ 256 ];
		static signed int ms_GVTable[ 256 ];
		static signed int ms_RVTable[ 256 ];
	};
}	// namespace Menge