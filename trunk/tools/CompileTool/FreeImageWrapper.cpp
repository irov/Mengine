#	include "FreeImageWrapper.h"
#	include <float.h>
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
bool FreeImageWrapper::CropImage(FIBITMAP * &_image, rectangle bbox)
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
void FreeImageWrapper::MakeAverage( FIBITMAP * _image )
{
	if(FreeImage_GetBPP(_image) != 32)
	{
		return;
	}

	int bytespp = FreeImage_GetLine(_image) / FreeImage_GetWidth(_image); 

	for(int y = 1; y < FreeImage_GetHeight(_image) -1; y++)
	{ 
		BYTE * bits = FreeImage_GetScanLine(_image, y); 
		BYTE * left_bits = bits - bytespp;
		BYTE * right_bits = bits + bytespp;


		BYTE * up_bits = FreeImage_GetScanLine(_image, y-1); //up
		BYTE * down_bits = FreeImage_GetScanLine(_image, y+1); //down
		
		for(int x = 1; x < FreeImage_GetWidth(_image) - 1; x++) 
		{
			//if(bits[FI_RGBA_ALPHA] <= Utility::AlphaMin)
			{
				unsigned char _colorR = 0;
				unsigned char _colorG = 0;
				unsigned char _colorB = 0;

				int count = 0;

				if(up_bits[FI_RGBA_ALPHA] != 0)
				{
					_colorR+=up_bits[FI_RGBA_RED];
					_colorG+=up_bits[FI_RGBA_GREEN];
					_colorB+=up_bits[FI_RGBA_BLUE];
					count++;
				}

				if(down_bits[FI_RGBA_ALPHA] != 0)
				{
					_colorR+=down_bits[FI_RGBA_RED];
					_colorG+=down_bits[FI_RGBA_GREEN];
					_colorB+=down_bits[FI_RGBA_BLUE];
					count++;
				}

				if(left_bits[FI_RGBA_ALPHA] != 0)
				{
					_colorR+=left_bits[FI_RGBA_RED];
					_colorG+=left_bits[FI_RGBA_GREEN];
					_colorB+=left_bits[FI_RGBA_BLUE];
					count++;
				}

				if(right_bits[FI_RGBA_ALPHA] != 0)
				{
					_colorR+=right_bits[FI_RGBA_RED];
					_colorG+=right_bits[FI_RGBA_GREEN];
					_colorB+=right_bits[FI_RGBA_BLUE];
					count++;
				}

				if(count)
				{
					_colorR = _colorR/count;
					_colorG = _colorG/count;
					_colorB = _colorB/count;
					bits[FI_RGBA_RED] = _colorR;
					bits[FI_RGBA_GREEN] = _colorG;
					bits[FI_RGBA_BLUE] = _colorB;
				}
			}

		/*	bits[FI_RGBA_RED] = _color; 
			bits[FI_RGBA_GREEN] = _color; 
			bits[FI_RGBA_BLUE] = _color; 
			bits[FI_RGBA_ALPHA] = _color;
			*/
			bits += bytespp; 

			up_bits += bytespp;
			down_bits += bytespp;
			left_bits += bytespp;
			right_bits += bytespp;
		} 
	}
}
//////////////////////////////////////////////////////////////////////////
void FreeImageWrapper::CorrectQuantinaze(int corner, FIBITMAP * _image, int _x, int _y, int _width, int _height)
{
	int bytespp = FreeImage_GetLine(_image) / FreeImage_GetWidth(_image); 

	//int w = _width - 2;
	int cy = FreeImage_GetHeight(_image) - _y;

	if(corner==0)
	{
		for(int y = cy-_height; y < cy; y++)
		{
			BYTE * original_bits = FreeImage_GetScanLine(_image, y);
			BYTE * neighboor_bits = original_bits + (_x+_width-1)*bytespp;

			original_bits+= (_x+_width)*bytespp;

			original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
			original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
			original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 
			original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];
	
		/*	original_bits[FI_RGBA_RED] = 0xFFAADD00; 
			original_bits[FI_RGBA_GREEN] = 0x00AADDFF; 
			original_bits[FI_RGBA_BLUE] = 0x00AADD00; 
			original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;
			
			neighboor_bits[FI_RGBA_RED] = 0x00AADDFF; 
			neighboor_bits[FI_RGBA_GREEN] = 0x00AADDFF; 
			neighboor_bits[FI_RGBA_BLUE] = 0xFFAADD00; 
			neighboor_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;*/
			
		}
	}

	if(corner==1)
	{
		for(int y = cy-_height; y < cy; y++)
		{
			BYTE * original_bits = FreeImage_GetScanLine(_image, y);
			BYTE * neighboor_bits = original_bits + (_x)*bytespp;

			original_bits+= (_x-1)*bytespp;

			original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
			original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
			original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 
			original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];

		/*	original_bits[FI_RGBA_RED] = 0xFFAADD00; 
			original_bits[FI_RGBA_GREEN] = 0x00AADDFF; 
			original_bits[FI_RGBA_BLUE] = 0x00AADD00; 
			original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;*/

		}
	}

	if(corner==2)
	{
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
			original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];

		/*	original_bits[FI_RGBA_RED] = 0xFFAADD00; 
			original_bits[FI_RGBA_GREEN] = 0x00AADDFF; 
			original_bits[FI_RGBA_BLUE] = 0x00AADD00; 
			original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;*/


			original_bits+= bytespp;
			neighboor_bits+= bytespp;
		}
	}

	cy = FreeImage_GetHeight(_image) - _y;

	if(corner==3)
	{
		BYTE * original_bits = FreeImage_GetScanLine(_image, cy);
		BYTE * neighboor_bits = FreeImage_GetScanLine(_image, cy-1);

		neighboor_bits = neighboor_bits + _x*bytespp;
		original_bits = original_bits + _x*bytespp;

		for(int x = 0; x < _width; x++) 
		{
			original_bits[FI_RGBA_RED] = neighboor_bits[FI_RGBA_RED]; 
			original_bits[FI_RGBA_GREEN] = neighboor_bits[FI_RGBA_GREEN]; 
			original_bits[FI_RGBA_BLUE] = neighboor_bits[FI_RGBA_BLUE]; 
			original_bits[FI_RGBA_ALPHA] = neighboor_bits[FI_RGBA_ALPHA];

		/*	original_bits[FI_RGBA_RED] = 0xFFAADD00; 
			original_bits[FI_RGBA_GREEN] = 0x00AADDFF; 
			original_bits[FI_RGBA_BLUE] = 0x00AADD00; 
			original_bits[FI_RGBA_ALPHA] = 0xFFFFFFFF;*/

			original_bits+= bytespp;
			neighboor_bits+= bytespp;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool FreeImageWrapper::FindAlphaBoundingBox8(FIBITMAP * _texture, rectangle & _bbox)
{
	int width = FreeImage_GetWidth(_texture);
	int height = FreeImage_GetHeight(_texture);

	rectangle imageRect = {0, 0, width, height};

	rectangle boundingBox = {78768, 78768, -1, -1};

	bool found = false;

	int bytespp = FreeImage_GetLine(_texture) / width; 
	  
    for(int y = height - 1; y >= 0; y--)
	{ 
		BYTE * bits = FreeImage_GetScanLine(_texture, y); 
	  
		for(int x = 0; x < width; x++)
		{ 
		  	if(bits[0] != 0)
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
bool FreeImageWrapper::FindAlphaBoundingBox(FIBITMAP * _texture, rectangle & _bbox)
{
	int width = FreeImage_GetWidth(_texture);
	int height = FreeImage_GetHeight(_texture);

	rectangle imageRect = {0, 0, width, height};

	if(FreeImage_GetBPP(_texture) != 32)
	{
		_bbox = imageRect;
		return false;
	}

	rectangle boundingBox = {78768, 78768, -1, -1};

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