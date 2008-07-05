#	pragma once
#	include <d3dx9.h>
#	include "windows.h"
#	include "FreeImage.h"

class FreeImageWrapper
{
public:
	static unsigned char * GrabMemory(
		FIBITMAP * bitmap, FREE_IMAGE_FORMAT type, unsigned long & size, int flags = 0);

	static FIBITMAP * AllocateImage(int width, int height, int bpp);

	static bool CropImage(FIBITMAP * &_image, RECT bbox);

	static void FillChannel( FIBITMAP * _image, int _channel, unsigned char _color );
};