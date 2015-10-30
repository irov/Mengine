#pragma once
#include "png.h"
#include "Utils/Codec/ImageDecoder.h"

class ImageDecoderPVRTC : public Menge::ImageDecoder
{
	struct PVRTextureHeader
	{
		uint32_t    version;            
		uint32_t    flags;          
		uint64_t    pixelFormat;        
		uint32_t    colourSpace;        
		uint32_t    channelType;        
		uint32_t    height;         
		uint32_t    width;          
		uint32_t    depth;          
		uint32_t    numSurfaces;        
		uint32_t    numFaces;       
		uint32_t    numMipmaps;     
		uint32_t    metaDataSize; 
	};

public:
	ImageDecoderPVRTC();
	~ImageDecoderPVRTC();

public:
	bool _prepareData() override;

public:
	size_t _decode( void * _buffer, size_t _bufferSize ) override;

private:
	PVRTextureHeader m_pvrtc_ptr;
};
