//#	pragma once
//
//#	include "Codec/VideoDecoder.h"
//
//#   include "vpx/vpx_decoder.h"
//#   include "vpx/vp8dx.h"
//
//
//namespace Menge
//{
//    //////////////////////////////////////////////////////////////////////////
//	class VideoDecoderVPX
//		: public VideoDecoder
//	{
//	public:
//		VideoDecoderVPX();
//		~VideoDecoderVPX();
//
//	public:
//		bool _initialize() override;
//        
//    public:
//		size_t decode( void * _buffer, size_t _bufferSize ) override;
//	
//	public:	
//		bool eof() override;
//		//int sync( float _timing ) override;
//		float getTiming()  const override;
//		bool seek( float _timing ) override;
//
//    public:
//        void setPitch( size_t _pitch ) override;
//
//	public:		
//		EVideoDecoderReadState readNextFrame( float & _pts ) override;
//
//	protected:
//        vpx_codec_ctx_t m_codec;
//        
//        unsigned char * m_frame;
//        size_t m_frameSize;
//
//        vpx_codec_iter_t m_iterator;
//
//        size_t m_pitch;
//        float m_pts;
//	};
//}	// namespace Menge
