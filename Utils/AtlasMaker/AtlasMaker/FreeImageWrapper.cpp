#	include "FreeImageWrapper.h"
//////////////////////////////////////////////////////////////////////////
unsigned char * FreeImageWrapper::GrabMemory(
	FIBITMAP * bitmap, FREE_IMAGE_FORMAT type, unsigned long & size, int flags)
{
	FIMEMORY * memory = FreeImage_OpenMemory();

	bool result = FreeImage_SaveToMemory( type, bitmap, memory, flags );

	if(result == false)
	{
		return 0;
	}

	unsigned char * data = 0;

	result = FreeImage_AcquireMemory( memory, &data, &size );

	if(result == false)
	{
		return 0;
	}
	
	unsigned char * ourData = new unsigned char[size];

	memcpy( ourData, data, size );
	
	FreeImage_CloseMemory( memory );

	return ourData;
}
//////////////////////////////////////////////////////////////////////////
bool FreeImageWrapper::CropImage(FIBITMAP * &_image, RECT bbox)
{
	FIBITMAP * clampedTexture = FreeImage_Copy(
		_image, bbox.left, bbox.top, bbox.right, bbox.bottom);

	if(clampedTexture == NULL)
	{
		return false;
	}

	FreeImage_Unload(_image);

	_image = clampedTexture;

	return true;
}
//////////////////////////////////////////////////////////////////////////
FIBITMAP * FreeImageWrapper::AllocateImage(int width, int height, int bpp)
{
	FIBITMAP * image = FreeImage_AllocateT(FIT_BITMAP,width,height);

	if(image == NULL)
	{
		return 0;
	}

	FIBITMAP * convertedImage = 0;

	if((bpp == 32) || (bpp == 64))
	{
		convertedImage = FreeImage_ConvertTo32Bits(image);
	}
	else if(bpp == 24)
	{
		convertedImage = FreeImage_ConvertTo24Bits(image);
	} 
	else if(bpp == 8)
	{
		convertedImage = FreeImage_ConvertTo8Bits(image);
	}
	else
	{
		return 0;
	}

	FreeImage_Unload(image);

	return convertedImage;
}
//////////////////////////////////////////////////////////////////////////
void FreeImageWrapper::FillChannel( FIBITMAP * _image, int _channel, unsigned char _color )
{
	if((FreeImage_GetBPP(_image) != 32) && _channel == FI_RGBA_ALPHA)
	{
		return;
	}

	int bytespp = FreeImage_GetLine(_image) / FreeImage_GetWidth(_image); 
  
	for(int y = 0; y < FreeImage_GetHeight(_image); y++)
	{ 
		BYTE * bits = FreeImage_GetScanLine(_image, y); 

		for(int x = 0; x < FreeImage_GetWidth(_image); x++) 
		{
			bits[_channel] = _color; 
			bits += bytespp; 
		} 
	}
}
//////////////////////////////////////////////////////////////////////////
void FreeImageWrapper::CorrectQuantinaze(FIBITMAP * _image, int _x, int _y, int _width, int _height)
{
	int bytespp = FreeImage_GetLine(_image) / FreeImage_GetWidth(_image); 

	//int w = _width - 2;

	int cy = FreeImage_GetHeight(_image) - _y;
	for(int y = cy-_height; y < cy; y++)
	{
		BYTE * original_bits = FreeImage_GetScanLine(_image, y);
		BYTE * neighboor_bits = original_bits + (_x+_width-1)*bytespp;

		original_bits+= (_x+_width)*bytespp;

	
		original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
		original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
		original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 
	//	original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];

	//	original_bits[FI_RGBA_RED] = 0x00AADD00; 
	}


	for(int y = cy-_height; y < cy; y++)
	{
		BYTE * original_bits = FreeImage_GetScanLine(_image, y);
		BYTE * neighboor_bits = original_bits + (_x)*bytespp;

		original_bits+= (_x-1)*bytespp;

		original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
		original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
		original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 
		//original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF; 
	}

	cy = FreeImage_GetHeight(_image) - _y - _height;
	
	BYTE * original_bits = FreeImage_GetScanLine(_image, cy-1);
	BYTE * neighboor_bits = FreeImage_GetScanLine(_image, cy);

	neighboor_bits = neighboor_bits + _x*bytespp;
	original_bits = original_bits + _x*bytespp;

		for(int x = 0; x < _width; x++) 
		{
			original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
			original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
			original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 

	//		original_bits[FI_RGBA_RED] = 0x00AADD00; 

			original_bits+= bytespp;
			neighboor_bits+= bytespp;
		}


		cy = FreeImage_GetHeight(_image) - _y;

		{
		
		
		BYTE * original_bits = FreeImage_GetScanLine(_image, cy+1);
		BYTE * neighboor_bits = FreeImage_GetScanLine(_image, cy);

		neighboor_bits = neighboor_bits + _x*bytespp;
		original_bits = original_bits + _x*bytespp;

		for(int x = 0; x < _width; x++) 
		{
			original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
			original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
			original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 

		/*	original_bits[FI_RGBA_RED] = 0x00AADD00; 
			original_bits[FI_RGBA_GREEN] = 0x00AADD00;
			original_bits[FI_RGBA_BLUE] = 0x00AADD00;
			original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;
*/
			original_bits+= bytespp;
			neighboor_bits+= bytespp;
		}
		}
		//	original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];
}
//////////////////////////////////////////////////////////////////////////
bool FreeImageWrapper::FindAlphaBoundingBox(FIBITMAP * _texture, RECT & _bbox)
{
	int width = FreeImage_GetWidth(_texture);
	int height = FreeImage_GetHeight(_texture);

	RECT imageRect = {0, 0, width, height};

	if(FreeImage_GetBPP(_texture) != 32)
	{
		_bbox = imageRect;
		return false;
	}

	RECT boundingBox = {INT_MAX, INT_MAX, -1, -1};

	bool found = false;

	int bytespp = FreeImage_GetLine(_texture) / width; 
	  
    for(int y = height - 1; y >= 0; y--)
	{ 
		BYTE * bits = FreeImage_GetScanLine(_texture, y); 
	  
		for(int x = 0; x < width; x++)
		{ 
		  	if(bits[FI_RGBA_ALPHA] != 0)
			{
				int cy = height - 1 - y;

				if (cy < boundingBox.top) boundingBox.top = cy;
				if (cy > boundingBox.bottom) boundingBox.bottom = cy;

				if (x < boundingBox.left) boundingBox.left = x;
				if (x > boundingBox.right) boundingBox.right = x;

				found = true;
			}

			bits += bytespp; 
		} 
	}

	boundingBox.right++;
	boundingBox.bottom++;

	if(found == false)
	{
		boundingBox = imageRect;
	}

	_bbox = boundingBox;

	return true;
}
//////////////////////////////////////////////////////////////////////////
int FreeImageWrapper::getPow2Size(int _size)
{
	int pow2size = 1;

	while(pow2size < _size)
	{
		pow2size <<= 1;
	}

	return pow2size;
}
//////////////////////////////////////////////////////////////////////////