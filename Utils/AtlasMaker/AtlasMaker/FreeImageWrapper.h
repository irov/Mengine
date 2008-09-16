#	pragma once
#	include <d3dx9.h>
#	include "windows.h"
#	include "FreeImage.h"

class FreeImageWrapper
{
public:
	// grabs bitmap into memory with specified bitmap format
	static unsigned char * GrabMemory(
		FIBITMAP * bitmap, FREE_IMAGE_FORMAT type, unsigned long & size, int flags = 0);
	// allocate image
	static FIBITMAP * AllocateImage(int width, int height, int bpp);
	// crop image by bounding box
	static bool CropImage(FIBITMAP * &_image, RECT bbox);
	// fill channel with specified color
	static void FillChannel( FIBITMAP * _image, int _channel, unsigned char _color );
	// find alpha bounding box around non alpha region
	static bool FindAlphaBoundingBox(FIBITMAP * _texture, RECT & _bbox);

	static void CorrectQuantinaze(FIBITMAP * _image, int _x, int _y, int _width, int _height);

	static int getPow2Size(int _size);
};