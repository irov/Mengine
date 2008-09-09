#	pragma once

//#include "Codec.h"
#	include "Interface/CodecInterface.h"
#include "PixelFormat.h"

namespace Menge 
{
	enum DecodeFlag
	{
		DF_NONE = 0x00000000,
		DF_COUNT_ALPHA = 0x00000001,
		DF_READ_ALPHA_ONLY = 0x00000002
	};
	
	class ImageCodec 
		: public CodecInterface
	{
	public:
		virtual ~ImageCodec()
		{
		}
	
		class ImageData 
			: public CodecInterface::CodecData
		{
		public:
			ImageData()
			: height( 0 )
			, width( 0 )
			, depth( 1 )
			, size( 0 )
			, num_mipmaps( 0 )
			, flags( 0 )
			, format( PF_UNKNOWN )
			{
			}
			
			std::size_t height;
			std::size_t width;
			std::size_t depth;
			std::size_t size;

			unsigned short num_mipmaps;
			unsigned int flags;

			PixelFormat format;
		};
	};

} // namespace Menge
