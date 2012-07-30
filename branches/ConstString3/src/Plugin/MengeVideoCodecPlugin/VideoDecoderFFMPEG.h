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
		//int sync( float _timing ) override;
		float getTiming()  const override;
		bool seek( float _timing ) override;

	public:
		bool isValid() const;
		void setOptions( CodecOptions * _options );
		
		EVideoDecoderReadState readNextFrame( ) override;
		const VideoCodecDataInfo* getCodecDataInfo() const;

	protected:
		void _invalidate();

	protected:
		bool seekFrame_( int64_t _frame );
	
	protected:
		LogServiceInterface * m_logService;
		AVFormatContext * m_formatContext;
		AVCodecContext * m_codecContext;
		AVCodec * m_codec;
		
		AVFrame * m_Frame; 
		AVFrame * m_FrameRGBA;
		AVIOContext * m_IOContext;
				
		AVInputFormat * m_inputFormat;
		
		VideoCodecDataInfo m_dataInfo;
		int m_outputPixelFormat;

		uint8_t * m_bufferIO;
		int m_probeSize;
		int m_videoStreamId;
		
		//float m_timing;
		bool m_isCompile;
		float m_pts;
		
	private:
		void clear_();
	};
}	// namespace Menge
