#	include "MengeCodec.h"
#	include "Codec.h"
//#	include "FreeImage.h"
#	include "FileEngine.h"

namespace Menge
{
	MengeCodec* MengeCodec::msInstance = 0;
	//////////////////////////////////////////////////////////////////////////
	MengeCodec::MengeCodec()
		: mType("mne")
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MengeCodec::~MengeCodec()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* MengeCodec::code(DataStreamInterface* input, CodecData* pData) const
	{
		assert(!"not implemented!");
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
    void MengeCodec::codeToFile(DataStreamInterface* input, const String& outFileName, CodecData* pData) const
	{
		assert(!"not implemented!");
	}
	//////////////////////////////////////////////////////////////////////////
	Codec::DecodeResult MengeCodec::decode(DataStreamInterface* _input) const
	{
	/*	unsigned long is_alpha = 0;
		_input->read(&is_alpha,sizeof(unsigned long));

		unsigned long jpg_size = 0;
		_input->read(&jpg_size,sizeof(unsigned long));

		unsigned char * jpg_buffer = (unsigned char*)_input->getBuffer();
		jpg_buffer += 2*sizeof(unsigned long);

		FIMEMORY * fiMemRGBImage = FreeImage_OpenMemory( jpg_buffer, jpg_size ); 
		FIBITMAP * fiBitmap = FreeImage_LoadFromMemory( FIF_JPEG, fiMemRGBImage );
		
		if(is_alpha == 1)
		{
			_input->seek(jpg_size+2*sizeof(unsigned long));

			unsigned long png_size = 0;
			_input->read(&png_size,sizeof(unsigned long));

			unsigned char * png_buffer = (unsigned char*)_input->getBuffer();
			png_buffer += (jpg_size+3*sizeof(unsigned long));

			FIMEMORY * fiMemAlphaImage = FreeImage_OpenMemory(png_buffer, png_size);
			FIBITMAP * fiAlpha = FreeImage_LoadFromMemory( FIF_PNG, fiMemAlphaImage );

			FIBITMAP * fiMemRGBImage32 = FreeImage_ConvertTo32Bits(fiBitmap);
			FreeImage_Unload( fiBitmap );

			fiBitmap = fiMemRGBImage32;

			BOOL res = FreeImage_SetChannel(fiMemRGBImage32,fiAlpha,FICC_ALPHA);

			FreeImage_Unload( fiAlpha );
			FreeImage_CloseMemory( fiMemAlphaImage );
		}

		if( fiBitmap == 0 )	// can't decode
		{
			DecodeResult res;
			res.first = 0;
			res.second = 0;
			return res;
		}

		ImageData* imgData = new ImageData();
		DataStreamInterface* output;

		imgData->depth = 1; // only 2D formats handled by this codec
		imgData->width = FreeImage_GetWidth(fiBitmap);
		imgData->height = FreeImage_GetHeight(fiBitmap);
		imgData->num_mipmaps = 0; // no mipmaps in non-DDS 
		imgData->flags = 0;

		// Must derive format first, this may perform conversions

		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
		FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(fiBitmap);
		unsigned bpp = FreeImage_GetBPP(fiBitmap);

		switch(imageType)
		{
		case FIT_UNKNOWN:
		case FIT_COMPLEX:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_DOUBLE:
		default:
			assert( 0 && "FreeImageCodec::decode -> Unknown or unsupported image format" );
			break;
		case FIT_BITMAP:
			// Standard image type
			// Perform any colour conversions for greyscale
			if (colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}
			// Perform any colour conversions for RGB
			else if (bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}

			// by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
			switch(bpp)
			{
			case 8:
				imgData->format = PF_L8;
				break;
			case 16:
				// Determine 555 or 565 from green mask
				// cannot be 16-bit greyscale since that's FIT_UINT16
				if(FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
				{
					imgData->format = PF_R5G6B5;
				}
				else
				{
					// FreeImage doesn't support 4444 format so must be 1555
					imgData->format = PF_A1R5G5B5;
				}
				break;
			case 24:
				// FreeImage differs per platform
				//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
				//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
				imgData->format = PF_BYTE_RGB;
#else
				imgData->format = PF_BYTE_BGR;
#endif
				break;
			case 32:
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
				imgData->format = PF_BYTE_RGBA;
#else
				imgData->format = PF_BYTE_BGRA;
#endif
				break;


			};
			break;
		case FIT_UINT16:
		case FIT_INT16:
			// 16-bit greyscale
			imgData->format = PF_L16;
			break;
		case FIT_FLOAT:
			// Single-component floating point data
			imgData->format = PF_FLOAT32_R;
			break;
		case FIT_RGB16:
			imgData->format = PF_SHORT_RGB;
			break;
		case FIT_RGBA16:
			imgData->format = PF_SHORT_RGBA;
			break;
		case FIT_RGBF:
			imgData->format = PF_FLOAT32_RGB;
			break;
		case FIT_RGBAF:
			imgData->format = PF_FLOAT32_RGBA;
			break;


		};

		unsigned char * srcData = FreeImage_GetBits(fiBitmap);
		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

		// Final data - invert image and trim pitch at the same time
		size_t dstPitch = imgData->width * PixelUtil::getNumElemBytes(imgData->format);
		imgData->size = dstPitch * imgData->height;
		// Bind output buffer
		unsigned char * data = new unsigned char[imgData->size];
		output = Holder<FileEngine>::hostage()->createMemoryFile( data, imgData->size, true );

		unsigned char * pSrc = 0;
		//unsigned char* pDst = static_cast<unsigned char*>( output->getBuffer() );
		unsigned char * pDst = data;
		for( std::size_t y = 0; y < imgData->height; ++y )
		{
			pSrc = srcData + (imgData->height - y - 1) * srcPitch;
			memcpy( pDst, pSrc, dstPitch );
			pDst += dstPitch;
		}


		FreeImage_Unload( fiBitmap );
		FreeImage_CloseMemory( fiMemRGBImage );

		DecodeResult ret;
		ret.first = output;
		ret.second = static_cast<CodecData*>( imgData );
		return ret;*/
		DecodeResult ret;
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////    
    String MengeCodec::getType() const
	{
		return mType;
	}
	//////////////////////////////////////////////////////////////////////////
	void MengeCodec::startup(void)
	{
		if (!msInstance)
		{
			msInstance = new MengeCodec();
			Codec::registerCodec(msInstance);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MengeCodec::shutdown(void)
	{
		if(msInstance)
		{
			Codec::unRegisterCodec(msInstance);
			delete msInstance;
			msInstance = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
};
