#include "ImageDecoderPVRTC.h"
#include "Utils/Logger/Logger.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/RenderSystemInterface.h"

#	include "Core/PixelFormat.h"

#define PNG_BYTES_TO_CHECK 8

#define _MAX( a, b ) (( a <= b )? b : a)

enum PixelType
{
	PVRTC2RGB = 0,
	PVRTC2RGBA,
	PVRTC4RGB,
	PVRTC4RGBA,
	PVRTCII2,
	PVRTCII4,
	ETC1,
	DXT1,
	DXT2,
	DXT3,
	DXT4,
	DXT5,
	BC1,
	BC2,
	BC3,
	BC4,
	BC5,
	BC6,
	BC7,
	UYVY,
	YUY2,
	BW1bpp,
	R9G9B9E5,
	RGBG8888,
	GRGB8888,
	ETC2RGB,
	ETC2RGBA,
	ETC2RGBA1,
	EACR11U,
	EACR11S,
	EACRG11U,
	EACRG11S,
};

const unsigned int PVRTEX_MIPMAP = (1<<8); // has mip map levels
const unsigned int PVRTEX_TWIDDLE = (1<<9); // is twiddled
const unsigned int PVRTEX_BUMPMAP = (1<<10); // has normals encoded for a bump map
const unsigned int PVRTEX_TILING = (1<<11); // is bordered for tiled pvr
const unsigned int PVRTEX_CUBEMAP = (1<<12); // is a cubemap/skybox
const unsigned int PVRTEX_FALSEMIPCOL  = (1<<13); //
const unsigned int PVRTEX_VOLUME = (1<<14);
const unsigned int PVRTEX_PIXELTYPE = 0xff; // pixel type is always in the last 16bits of the flags
const unsigned int PVRTEX_IDENTIFIER  = 0x21525650;  // the pvr identifier is the characters 'P','V','R'

const unsigned int PVRTEX_V1_HEADER_SIZE = 44; // old header size was 44 for identification purposes

//////////////////////////////////////////////////////////////////////////
ImageDecoderPVRTC::ImageDecoderPVRTC()
	: ImageDecoder()
{
}
//////////////////////////////////////////////////////////////////////////
ImageDecoderPVRTC::~ImageDecoderPVRTC()
{
}
//////////////////////////////////////////////////////////////////////////
size_t ImageDecoderPVRTC::decode( void * _buffer, size_t _bufferSize )
{
	// TODO: investigate why sizeof(PVRTextureHeader) != 52
	m_stream->seek(52 + m_pvrtc_ptr.metaDataSize);
	size_t read = m_stream->read(_buffer, _bufferSize);

	return read == _bufferSize ? read : 0;
}
//////////////////////////////////////////////////////////////////////////
bool ImageDecoderPVRTC::_prepareData()
{
	m_stream->seek(0);
	
	m_stream->read(&m_pvrtc_ptr, 52);

	switch (m_pvrtc_ptr.pixelFormat)
	{
		case PVRTC4RGB:
			{
				m_dataInfo.format = Menge::PF_PVRTC4_RGB;
			}break;
		case PVRTC4RGBA:
			{				
				m_dataInfo.format = Menge::PF_PVRTC4_RGBA;
			}break;
		case ETC1:
			{
				m_dataInfo.format = Menge::PF_ETC1;
			}break;
		default:
			return false; // not supported format
	}
	
	m_dataInfo.width = m_pvrtc_ptr.width;
	m_dataInfo.height = m_pvrtc_ptr.height;
	m_dataInfo.channels = 3;
	m_dataInfo.depth = 1;
	m_dataInfo.mipmaps = m_pvrtc_ptr.numMipmaps;
	m_dataInfo.size = Menge::Helper::getTextureMemorySize( m_dataInfo.width, m_dataInfo.height, m_dataInfo.channels, m_dataInfo.depth, m_dataInfo.format );
	
	bool isValid = m_pvrtc_ptr.numFaces == 1  // supported only 1 face
		&& m_pvrtc_ptr.numSurfaces == 1  // supported only 1 surface
		&& m_dataInfo.mipmaps < 2 ; // mipmapping is disabled, so is not supported.
	
	return isValid;
}
