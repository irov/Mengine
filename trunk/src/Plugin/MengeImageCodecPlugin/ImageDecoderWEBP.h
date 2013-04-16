///*
// *	ImageDecoderPNG.h
// *
// *	Created by _Berserk_ on 18.12.2008
// *	Copyright 2008 Menge. All rights reserved.
// *
// */
//
//#	pragma once
//
//#	include "Codec/ImageDecoder.h"
//#	include "webp/decode.h"
//#	include "webp/mux.h"
//
//namespace Menge
//{
//	class ImageDecoderWEBP
//		: public ImageDecoder
//	{
//	public:
//		ImageDecoderWEBP( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem );
//		~ImageDecoderWEBP();
//
//	public:
//		bool initialize() override;
//
//	public:
//		LogSystemInterface * getLogSystem();
//		void setOptions( CodecOptions * _options );
//	public:
//		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
//
//	protected:
//		void _invalidate() override;
//	protected:
//		WebPMux* m_mux;
//		uint8_t* m_dataBuffer;
//		bool m_isValid;
//		bool m_isAlpha;
//		size_t m_bufferRowStride;
//	private:
//		LogSystemInterface * m_logSystem;
//		bool m_supportA8;
//		void cleanup_();
//		
//	};
//}	// namespace Menge
