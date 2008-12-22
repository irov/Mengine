#	pragma once

#	include "ImageCodec.h"
#	include "libPNG/png.h"

namespace Menge
{
	class ImageCodecPNG
		: public ImageCodec
	{
		MENGE_DECLARE_CODEC( ImageCodecPNG );

	public:
		ImageCodecPNG();
		~ImageCodecPNG();

	public:

		void destructor() override;
		void release() override;
		const String& getType() const override;

		void setDecodeOptions( unsigned int _options ) override;
		const CodecData* startDecode( DataStreamInterface* _stream ) override;
		void decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		bool eof() const override;
		void finishDecode() override;

		void startEncode( OutStreamInterface* _stream ) override;
		void encode( unsigned char* _buffer, CodecData* _codecData ) override;
		void finishEncode() override;

	/*	bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const override;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const override;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const override;

		bool start( DataStreamInterface* _inputData, CodecData* _codecData ) override;
		int sync( float _timing ) override;
		void finish() override;

		std::streamsize read( void* _buf, std::streamsize _count ) override;
		bool eof() const override; */
	protected:
		ImageCodecData m_codecData;
		DataStreamInterface* m_inputStream;
		png_structp m_decode_png_ptr;
		bool m_decodeStarted;
		unsigned int m_decodeOptions;
		unsigned int m_decodeRowStride;
		unsigned int m_decodeBufferRowStride;

		void decodeCleanup_();

		String m_type;
	};
}	// namespace Menge
