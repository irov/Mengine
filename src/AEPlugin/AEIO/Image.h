#	pragma once

#	include <stdint.h>

namespace Menge
{
	unsigned char * imageScale( unsigned char * _image, uint32_t _widthOld, uint32_t _heightOld, uint32_t _pitchOld, uint32_t _widthNew, uint32_t _heightNew, uint32_t _pitchNew, uint32_t _channel );
	unsigned char * imageLoadFromMemory( const void * _data, size_t _size, uint32_t & _width, uint32_t & _height );
	unsigned char * imageLoadFromFile( const char * _fileName, uint32_t & _width, uint32_t & _height );

	void imageFree( unsigned char * _free );
}