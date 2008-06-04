#	pragma once

#include "Codec.h"
#include "PixelFormat.h"

namespace Menge 
{
	//Codec specialized in images.
	//@remarks
	//The users implementing subclasses of ImageCodec are required to return
	//a valid pointer to a ImageData class from the decode(...) function.
	class ImageCodec : public Codec
	{
	public:
		virtual ~ImageCodec();
		//Codec return class for images. Has imformation about the size and the
		//pixel format of the image.
		class ImageData : public Codec::CodecData
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

		public:
			std::string dataType() const
			{
				return "ImageData";
			}
		};

	public:
		std::string getDataType() const
		{
			return "ImageData";
		}
	};

} // namespace Menge
