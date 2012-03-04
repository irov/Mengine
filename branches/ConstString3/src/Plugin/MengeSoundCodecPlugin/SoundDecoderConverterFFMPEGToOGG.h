///*
// *	SoundDecoderConverterFFMPEGToOGG.h
// *	Copyright 2011 Menge. All rights reserved.
// *
// */
//
//#	pragma once
//#	include "SoundDecoderConverter.h"
//
//namespace Menge
//{
//	class LogServiceInterface;
//
//	class SoundDecoderConverterFFMPEGToOGG
//		: public SoundDecoderConverter
//	{
//	public:
//		SoundDecoderConverterFFMPEGToOGG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logSystem, FileServiceInterface * _fileService );
//		~SoundDecoderConverterFFMPEGToOGG();
//
//	public:
//		bool initialize() override;
//		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
//		
//	public:
//		bool isValid() const;
//
//		const SoundCodecDataInfo* getCodecDataInfo() const;
//		void setOptions( CodecOptions * ) ;
//		
//		bool seek( float _timing ) override;
//		float timeTell() override;
//
//	protected:
//		void _invalidate() ;
//
//	protected:
//		LogServiceInterface * m_logService;
//		SoundCodecDataInfo m_dataInfo;
//		FileInputStreamInterface * m_convertedStream;
//		SoundDecoderInterface * m_decoder;
//		bool m_isValid;
//
//	private:
//		bool convert_( const String & input, const String & output );
//		void clear_();
//	};
//}	// namespace Menge
