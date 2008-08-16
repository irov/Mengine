#	pragma once

#	include "ImageCodec.h"

namespace Menge
{
	class ImageCodecPNG
		: public ImageCodec
	{
	public:
		ImageCodecPNG(const String & _type);
		~ImageCodecPNG();
	public:
		bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const override;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const override;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const override;
		const String & getType() const override;
	private:
		String m_type;
	};
}	// namespace Menge