
#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
//#	include "PixelFormat.h"

namespace Menge
{
	struct ImageCodecDataInfo
		: public CodecDataInfo
	{
		ImageCodecDataInfo()
			: height( 0 )
			, width( 0 )
			, depth( 1 )
			, num_mipmaps( 0 )
			, flags( 0 )
			, format( PF_UNKNOWN )
			, quality( 100 )
		{
		}

		std::size_t height;
		std::size_t width;
		std::size_t depth;

		unsigned short num_mipmaps;
		unsigned int flags;

		PixelFormat format;

		int quality;
	};

	enum ImageDecoderOptions
	{
		DF_NONE = 0x00000000,
		DF_COUNT_ALPHA = 0x00000001,
		DF_READ_ALPHA_ONLY = 0x00000002,
		DF_CUSTOM_PITCH = 0x00000004,
		DF_QUALITY = 0x00000008
	};

	class ImageDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual void setOptions( unsigned int _options ) = 0;
	};

	class ImageEncoderInterface
		: public EncoderInterface
	{
	public:
		virtual void setOptions( unsigned int _options ) = 0;
	};
}	// namespace Menge
