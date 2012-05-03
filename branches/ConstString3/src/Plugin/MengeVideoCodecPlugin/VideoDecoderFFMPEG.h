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
	class LogServiceInterface;

	class VideoDecoderFFMPEG
		: public VideoDecoder
	{
	public:
		VideoDecoderFFMPEG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logService );
		~VideoDecoderFFMPEG();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		
	public:
		
		bool eof() override;
		int sync( float _timing ) override;
		float getTiming()  const override;
		bool seek( float _timing ) override;
		bool isValid() const;
		const VideoCodecDataInfo* getCodecDataInfo() const;
		void setOptions( CodecOptions * ) ;
		bool readNextFrame( ) override;
	protected:
void _invalidate() ;
	protected:
		LogServiceInterface * m_logService;
		AVFormatContext * m_formatContext;
		AVCodecContext * m_codecContext;
		AVCodec * m_codec;
		
		size_t * m_cacheBuffer;
		
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
		float m_frameTiming;
		bool m_isValid;
		float m_timing;

		float m_pts;
		
	private:
		void clear_();
		
	};
}	// namespace Menge
