
#include "FreeImageCodec.h"
#include "Image.h"
#	include "LogEngine.h"

#include <FreeImage.h>

#	include <sstream>

namespace Menge
{
	FreeImageCodec::TRegisteredCodecList FreeImageCodec::ms_codecList;
	//////////////////////////////////////////////////////////////////////////
	void FreeImageErrorHandler( FREE_IMAGE_FORMAT _fif, const char* _message ) 
	{
		// Callback method as required by FreeImage to report problems
		std::ostringstream str;
		str << "FreeImage error: '" << _message << "'";

		const char* typeName = FreeImage_GetFormatFromFIF( _fif );
		if( typeName )
		{
			str << " when loading format " << typeName;
		}

		MENGE_LOG( str.str().c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FreeImageCodec::startup()
	{
		FreeImage_Initialise(false);

		MENGE_LOG( String( "FreeImage version: " + String( FreeImage_GetVersion() ) ).c_str() );
		MENGE_LOG( FreeImage_GetCopyrightMessage() );

		// Register codecs
		std::ostringstream strExt;
		strExt << "Supported formats: ";
		bool first = true;
		for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
		{

			// Skip DDS codec since FreeImage does not have the option 
			// to keep DXT data compressed, we'll use our own codec
			if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
				continue;

			String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
			if (!first)
			{
				strExt << ",";
			}
			first = false;
			strExt << exts;

			ImageCodec* codec = new FreeImageCodec( exts, i );
			ms_codecList.push_back( codec );
			Codec::registerCodec( codec );
			// Pull off individual formats (separated by comma by FI)
			/*TStringVector extsVector = StringUtil::split(exts, ",");
			for( TStringVector::iterator v = extsVector.begin(), v_end = extsVector.end();
					v != v_end;
					++v )
			{
				ImageCodec* codec = new FreeImageCodec(*v, i);
				ms_codecList.push_back( codec );
				Codec::registerCodec( codec );
			}*/
		}
		MENGE_LOG( strExt.str().c_str() );

		// Set error handler
		FreeImage_SetOutputMessage( FreeImageErrorHandler );
	}
	//////////////////////////////////////////////////////////////////////////
	void FreeImageCodec::shutdown()
	{
		FreeImage_DeInitialise();

		for( TRegisteredCodecList::iterator it = ms_codecList.begin(), it_end = ms_codecList.end();
			it != it_end;
			++it )
		{
			Codec::unRegisterCodec( *it );
			delete (*it);
		}
		ms_codecList.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	FreeImageCodec::FreeImageCodec( const String& _type, unsigned int _fiType )
	: m_type( _type )
	, m_freeImageType( _fiType )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	FIBITMAP* FreeImageCodec::encode( DataStreamInterface* _input, CodecData* _data ) const
	{
		FIBITMAP* ret = 0;

		ImageData* pImgData = static_cast< ImageData * >( _data );
		PixelBox src(pImgData->width, pImgData->height, pImgData->depth, pImgData->format, _input->getBuffer() );

		// The required format, which will adjust to the format
		// actually supported by FreeImage.
		PixelFormat requiredFormat = pImgData->format;

		// determine the settings
		FREE_IMAGE_TYPE imageType;
		switch( pImgData->format )
		{
		case PF_R5G6B5:
		case PF_B5G6R5:
		case PF_R8G8B8:
		case PF_B8G8R8:
		case PF_A8R8G8B8:
		case PF_X8R8G8B8:
		case PF_A8B8G8R8:
		case PF_X8B8G8R8:
		case PF_B8G8R8A8:
		case PF_R8G8B8A8:
		case PF_A4L4:
		case PF_BYTE_LA:
		case PF_R3G3B2:
		case PF_A4R4G4B4:
		case PF_A1R5G5B5:
		case PF_A2R10G10B10:
		case PF_A2B10G10R10:
			// I'd like to be able to use r/g/b masks to get FreeImage to load the data
			// in it's existing format, but that doesn't work, FreeImage needs to have
			// data in RGB[A] (big endian) and BGR[A] (little endian), always.
			if (PixelUtil::hasAlpha(pImgData->format))
			{
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
				requiredFormat = PF_BYTE_RGBA;
#else
				requiredFormat = PF_BYTE_BGRA;
#endif
			}
			else
			{
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
				requiredFormat = PF_BYTE_RGB;
#else
				requiredFormat = PF_BYTE_BGR;
#endif
			}
			// fall through
		case PF_L8:
		case PF_A8:
			imageType = FIT_BITMAP;
			break;

		case PF_L16:
			imageType = FIT_UINT16;
			break;

		case PF_SHORT_GR:
			requiredFormat = PF_SHORT_RGB;
			// fall through
		case PF_SHORT_RGB:
			imageType = FIT_RGB16;
			break;

		case PF_SHORT_RGBA:
			imageType = FIT_RGBA16;
			break;

		case PF_FLOAT16_R:
			requiredFormat = PF_FLOAT32_R;
			// fall through
		case PF_FLOAT32_R:
			imageType = FIT_FLOAT;
			break;

		case PF_FLOAT16_GR:
		case PF_FLOAT16_RGB:
		case PF_FLOAT32_GR:
			requiredFormat = PF_FLOAT32_RGB;
			// fall through
		case PF_FLOAT32_RGB:
			imageType = FIT_RGBF;
			break;

		case PF_FLOAT16_RGBA:
			requiredFormat = PF_FLOAT32_RGBA;
			// fall through
		case PF_FLOAT32_RGBA:
			imageType = FIT_RGBAF;
			break;

		default:
			assert( 0 && "FreeImageCodec::encode -> Invalid image format" );
		};

		bool conversionRequired = false;

		unsigned char* srcData = static_cast<unsigned char*>( _input->getBuffer() );

		// Check BPP
		unsigned bpp = static_cast<unsigned>(PixelUtil::getNumElemBits(requiredFormat));
		if( !FreeImage_FIFSupportsExportBPP( (FREE_IMAGE_FORMAT)m_freeImageType, (int)bpp ) )
		{
			if (bpp == 32 && PixelUtil::hasAlpha(pImgData->format) && FreeImage_FIFSupportsExportBPP((FREE_IMAGE_FORMAT)m_freeImageType, 24))
			{
				// drop to 24 bit (lose alpha)
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
				requiredFormat = PF_BYTE_RGB;
#else
				requiredFormat = PF_BYTE_BGR;
#endif
				bpp = 24;
			}
		}

		PixelBox convBox(pImgData->width, pImgData->height, 1, requiredFormat);
		if (requiredFormat != pImgData->format)
		{
			conversionRequired = true;
			// Allocate memory
			convBox.data = new unsigned char[convBox.getConsecutiveSize()];
			// perform conversion and reassign source
			PixelBox src( pImgData->width, pImgData->height, 1, pImgData->format, _input->getBuffer() );
			PixelUtil::bulkPixelConversion( src, convBox );
			srcData = static_cast<unsigned char*>( convBox.data );

		}

		ret = FreeImage_AllocateT(
			imageType, 
			static_cast<int>(pImgData->width), 
			static_cast<int>(pImgData->height), 
			bpp);

		if (requiredFormat == PF_L8 || requiredFormat == PF_A8)
		{
			// Must explicitly tell FreeImage that this is greyscale by setting
			// a "grey" palette (otherwise it will save as a normal RGB
			// palettized image).
			FIBITMAP *tmp = FreeImage_ConvertToGreyscale(ret);
			FreeImage_Unload(ret);
			ret = tmp;
		}

		size_t dstPitch = FreeImage_GetPitch(ret);
		size_t srcPitch = pImgData->width * PixelUtil::getNumElemBytes(requiredFormat);


		// Copy data, invert scanlines and respect FreeImage pitch
		unsigned char* pSrc;
		unsigned char* pDst = FreeImage_GetBits(ret);
		for (size_t y = 0; y < pImgData->height; ++y)
		{
			pSrc = srcData + (pImgData->height - y - 1) * srcPitch;
			memcpy(pDst, pSrc, srcPitch);
			pDst += dstPitch;
		}

		if (conversionRequired)
		{
			// delete temporary conversion area
			delete [] static_cast<unsigned char*>(convBox.data);
		}

		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FreeImageCodec::code( DataStreamInterface* _input, Codec::CodecData* _data ) const
	{        
		FIBITMAP* fiBitmap = encode( _input, _data );

		// open memory chunk allocated by FreeImage
		FIMEMORY* mem = FreeImage_OpenMemory();
		// write data into memory
		FreeImage_SaveToMemory( (FREE_IMAGE_FORMAT)m_freeImageType, fiBitmap, mem );
		// Grab data information
		BYTE* data;
		DWORD size;
		FreeImage_AcquireMemory( mem, &data, &size );
		// Copy data into our own buffer
		BYTE* ourData = new BYTE[size];
		memcpy( ourData, data, size );
		// Wrap data in stream, tell it to free on close 
		DataStreamInterface* outstream = Holder<FileEngine>::hostage()->createMemoryFile( ourData, size, true );
		// Now free FreeImage memory buffers
		FreeImage_CloseMemory( mem );
		// Unload bitmap
		FreeImage_Unload( fiBitmap );

		return outstream;
	}
	//////////////////////////////////////////////////////////////////////////
	void FreeImageCodec::codeToFile( DataStreamInterface* _input, const String& _outFileName, 
		Codec::CodecData* _data ) const
	{
		FIBITMAP* fiBitmap = encode( _input, _data );

		FreeImage_Save( (FREE_IMAGE_FORMAT)m_freeImageType, fiBitmap, _outFileName.c_str() );
		FreeImage_Unload( fiBitmap );
	}
	//////////////////////////////////////////////////////////////////////////
	Codec::DecodeResult FreeImageCodec::decode( DataStreamInterface* _input ) const
	{
		// Buffer stream into memory (TODO: override IO functions instead?)
		//MemoryDataStream memStream( _input, true );

		FIMEMORY* fiMem = 
			FreeImage_OpenMemory( static_cast<unsigned char*>( _input->getBuffer() ), static_cast<DWORD>( _input->size() ) );

		FIBITMAP* fiBitmap = FreeImage_LoadFromMemory( (FREE_IMAGE_FORMAT)m_freeImageType, fiMem );

		//assert( fiBitmap && "FreeImageCodec::decode -> Error decoding image" );
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

		unsigned char* srcData = FreeImage_GetBits(fiBitmap);
		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

		// Final data - invert image and trim pitch at the same time
		size_t dstPitch = imgData->width * PixelUtil::getNumElemBytes(imgData->format);
		imgData->size = dstPitch * imgData->height;
		// Bind output buffer
		unsigned char* data = new unsigned char[imgData->size];
		output = Holder<FileEngine>::hostage()->createMemoryFile( data, imgData->size, true );

		unsigned char* pSrc;
		//unsigned char* pDst = static_cast<unsigned char*>( output->getBuffer() );
		unsigned char* pDst = data;
		for( std::size_t y = 0; y < imgData->height; ++y )
		{
			pSrc = srcData + (imgData->height - y - 1) * srcPitch;
			memcpy( pDst, pSrc, dstPitch );
			pDst += dstPitch;
		}


		FreeImage_Unload( fiBitmap );
		FreeImage_CloseMemory( fiMem );

		DecodeResult ret;
		ret.first = output;
		ret.second = static_cast<CodecData*>( imgData );
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////    
	String FreeImageCodec::getType() const 
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
}
