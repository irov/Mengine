/*
 *	VideoDecoderFFMPEG.h
 *	Copyright 2011 Menge. All rights reserved.
 *
 */

#	pragma once

#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif
#	include "VideoDecoder.h"

extern "C"
{
	#	include "libavformat/avformat.h"
	#	include "libswscale/swscale.h"
	#	include "libavcodec/avcodec.h"
	#	include "libavutil/mathematics.h"
}

namespace Menge
{
	class LogSystemInterface;

	class VideoDecoderFFMPEG
		: public VideoDecoder
	{
	public:
		VideoDecoderFFMPEG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem );
		~VideoDecoderFFMPEG();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		
	public:
		
		bool eof() override;
		int sync( float _timing ) override;
		bool seek( float _timing ) override;
		bool isValid() const;
		const VideoCodecDataInfo* getCodecDataInfo() const;
		void setOptions( CodecOptions * ) ;
	
	protected:
void _invalidate() ;
	protected:
		LogSystemInterface * m_logSystem;
		AVFormatContext * m_formatContext;
		AVCodecContext * m_codecContext;
		AVCodec * m_codec;
		
		AVFrame * m_Frame; 
		AVFrame * m_FrameRGBA;
		
		ByteIOContext * m_byteIOContext;
		AVInputFormat * m_inputFormat;
		
		VideoCodecDataInfo m_dataInfo;
		int m_outputPixelFormat;

		uint8_t * m_bufferIO;
		int m_probeSize;
		int m_videoStreamId;
		
		int m_frameRate;
		bool m_isValid;
		bool m_eof;
		float m_timing;
		unsigned int m_curFrameTime;
	private:
		void clear_();
		bool readFrame_( );
	};
}	// namespace Menge
