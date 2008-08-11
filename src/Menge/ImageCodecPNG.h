#	pragma once

#	include "ImageCodec.h"

namespace Menge
{
	class ImageCodecPNG
		: public ImageCodec
	{
	public:
		ImageCodecPNG();

		String getType() const;
		DataStreamInterface* code( DataStreamInterface* input, CodecData* _data ) const;
		void codeToFile( DataStreamInterface* _input, const String& _outFileName, CodecData* _pData ) const;
		DecodeResult decode( DataStreamInterface* _input ) const;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const;

	};
}	// namespace Menge