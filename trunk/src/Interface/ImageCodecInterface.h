#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/PixelFormat.h"

#	define MENGINE_IMAGE_MIPLEVEL_MAX 16

namespace Menge
{
	struct ImageCodecDataInfo
		: public CodecDataInfo
	{
		ImageCodecDataInfo()
			: mipmaps(1)
			, width(0)
			, height(0)
			, channels(0)
			, depth(1)
			, quality(100)			
            , format(PF_UNKNOWN)
		{
		}

        uint32_t mipmaps;
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		uint32_t depth;
		
		int quality;

        PixelFormat format;

		size_t size[MENGINE_IMAGE_MIPLEVEL_MAX];

		size_t getFullSize() const
		{
			size_t full_size = 0;

			for( uint32_t i = 0; i != mipmaps; ++i )
			{
				size_t s = size[0];

				full_size += s;
			}

			return full_size;
		}
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
			, miplevel(0)
        {
        }

        uint32_t flags;
		size_t pitch;
        uint32_t channels;
		uint32_t miplevel;
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
