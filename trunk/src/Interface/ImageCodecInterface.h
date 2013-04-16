
#	pragma once

#	include "Interface/CodecInterface.h"
#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
	struct ImageCodecDataInfo
		: public CodecDataInfo
	{
		ImageCodecDataInfo()
			: width(0)
			, height(0)
			, depth(1)
			, num_mipmaps(0)
			, flags(0)
			, quality(100)
			, channels(0)
            , format(PF_UNKNOWN)
		{
		}

        size_t width;
		size_t height;
		
		size_t depth;

		unsigned short num_mipmaps;
		unsigned int flags;

		int quality;
		size_t channels;

        PixelFormat format;
	};

	struct ImageCodecOptions
		: public CodecOptions
	{
        ImageCodecOptions()
            : flags(0)
            , pitch(0)
            , channels(0)
        {
        }

        size_t flags;
		size_t pitch;
        size_t channels;
	};

	enum ImageDecoderOptions
	{
		DF_NONE = 0x00000000,
		DF_COUNT_ALPHA = 0x00000001,
		DF_READ_ALPHA_ONLY = 0x00000002,
		//DF_CUSTOM_PITCH = 0x00000004,
		DF_QUALITY = 0x00000008,
		DF_WRITE_ALPHA_ONLY = 0x00000010,
        DF_NOT_ADD_ALPHA = 0x00000020
	};

	class ImageDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const ImageCodecDataInfo * getCodecDataInfo() const override = 0;
	};

	class ImageEncoderInterface
		: public EncoderInterface
	{
	};
}	// namespace Menge
