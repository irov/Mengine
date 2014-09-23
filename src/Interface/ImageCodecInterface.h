#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/PixelFormat.h"

namespace Menge
{
	struct ImageCodecDataInfo
		: public CodecDataInfo
	{
		ImageCodecDataInfo()
			: width(0)
			, height(0)
			, depth(1)
			, mipmaps(0)
			, quality(100)
			, channels(0)
            , format(PF_UNKNOWN)
		{
		}

        uint32_t width;
		uint32_t height;
		
		uint32_t depth;
		uint32_t mipmaps;

		int quality;
		uint32_t channels;

        PixelFormat format;
	};

	enum ImageDecoderOptions
	{
		DF_NONE				= 0x00000000,
		DF_COUNT_ALPHA		= 0x00000001,
		DF_READ_ALPHA_ONLY	= 0x00000002,
		DF_QUALITY			= 0x00000004,
		DF_WRITE_ALPHA_ONLY = 0x00000008,
		DF_NOT_ADD_ALPHA	= 0x00000010
	};

	struct ImageCodecOptions
		: public CodecOptions
	{
        ImageCodecOptions()
            : flags(DF_NONE)
            , pitch(0)
            , channels(0)
        {
        }

        uint32_t flags;
		size_t pitch;
        uint32_t channels;
	};

	class ImageDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const ImageCodecDataInfo * getCodecDataInfo() const override = 0;
	};

    typedef stdex::intrusive_ptr<ImageDecoderInterface> ImageDecoderInterfacePtr;

	class ImageEncoderInterface
		: public EncoderInterface
	{
	};

    typedef stdex::intrusive_ptr<ImageEncoderInterface> ImageEncoderInterfacePtr;
}
