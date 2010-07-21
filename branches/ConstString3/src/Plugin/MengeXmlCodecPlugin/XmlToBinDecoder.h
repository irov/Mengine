#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class InputStreamInterface;

	class Xml2BinDecoder
		: public DecoderInterface
	{
	public:
		Xml2BinDecoder( InputStreamInterface * _stream );
	
	public:
		void setOptions( CodecOptions * _info ) override;

	public:
		const CodecDataInfo* getCodecDataInfo() const override;
		InputStreamInterface * getStream() const override;

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void release() override;

	protected:
		InputStreamInterface * m_stream;
	};
}