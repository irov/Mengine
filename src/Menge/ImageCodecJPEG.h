#	pragma once

#	include "ImageCodec.h"

namespace Menge
{
	class ImageCodecJPEG
		: public ImageCodec
	{
	public:
		ImageCodecJPEG();
		~ImageCodecJPEG();
	public:
		bool code( OutStreamInterface* _input, unsigned char* _buffer, CodecData* _data ) const override;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const override;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const override;

		bool start( DataStreamInterface* _inputData, CodecData* _codecData ) override;
		int sync( float _timing ) override;
		void finish() override;

		std::streamsize read( void* _buf, std::streamsize _count ) override;
		bool eof() const override;
/*		void seek( std::streamoff _pos ) override;
		void skip( std::streampos _count ) override;
		std::streampos tell() const override;
		std::streamsize size() const override;
		void* getBuffer() override;
		void setFreeOnClose( bool _free ) override;*/

	};
}	// namespace Menge