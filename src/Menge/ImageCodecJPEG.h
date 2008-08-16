#	pragma once

#	include "ImageCodec.h"

namespace Menge
{
	class ImageCodecJPEG
		: public ImageCodec
	{
	public:
		ImageCodecJPEG(const String & _type);
		~ImageCodecJPEG();
	public:
		bool code( OutStreamInterface* _input, unsigned char* _buffer, CodecData* _data ) const override;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const override;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const override;
		const String & getType() const override;
	private:
		String m_type;
	};
}	// namespace Menge