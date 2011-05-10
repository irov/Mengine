
#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	struct ImageCodecDataInfo
		: public CodecDataInfo
	{
		ImageCodecDataInfo()
			: height(0)
			, width(0)
			, depth(1)
			, num_mipmaps(0)
			, flags(0)
			, format(PF_UNKNOWN)
			, quality(100)
			, channels(0)
		{
		}

		std::size_t height;
		std::size_t width;
		std::size_t depth;

		unsigned short num_mipmaps;
		unsigned int flags;

		PixelFormat format;

		int quality;
		int channels;
	};

	struct ImageCodecOptions
		: public CodecOptions
	{
		int pitch;

		std::size_t begin_x;
		std::size_t begin_y;
		std::size_t end_x;
		std::size_t end_y;
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
		: virtual public DecoderInterface
	{
	public:
		virtual const ImageCodecDataInfo * getCodecDataInfo() const override = 0;
	};

	class ImageEncoderInterface
		: virtual public EncoderInterface
	{
	};
}	// namespace Menge
