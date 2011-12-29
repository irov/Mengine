/*
 *	SoundDecoderConverterFFMPEGToOGG.h
 *	Copyright 2011 Menge. All rights reserved.
 *
 */

#	pragma once
#	include "SoundDecoderConverter.h"

namespace Menge
{
	class LogSystemInterface;

	class SoundDecoderConverterFFMPEGToOGG
		: public SoundDecoderConverter
	{
	public:
		SoundDecoderConverterFFMPEGToOGG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem, FileServiceInterface * _fileService );
		~SoundDecoderConverterFFMPEGToOGG();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		
	public:
		bool isValid() const;
		const SoundCodecDataInfo* getCodecDataInfo() const;
		void setOptions( CodecOptions * ) ;
		
		bool seek( float _timing ) override;
		float timeTell() override;
	protected:
void _invalidate() ;
	protected:
		LogSystemInterface * m_logSystem;
		SoundCodecDataInfo m_dataInfo;
		InputStreamInterface * m_convertedStream;
		SoundDecoderInterface * m_decoder;
		bool m_isValid;
	private:
		bool convert_( const String & input, const String & output );
		void clear_();
	};
}	// namespace Menge
