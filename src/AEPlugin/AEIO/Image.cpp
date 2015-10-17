#	include "Image.h"

#	include "stb_image.c"

#	include <stdio.h>

namespace Menge
{
	unsigned char * imageScale( unsigned char * _image, uint32_t _widthOld, uint32_t _heightOld, uint32_t _pitchOld, uint32_t _widthNew, uint32_t _heightNew, uint32_t _pitchNew, uint32_t _channel )
	{
		uint32_t size = _widthNew * _heightNew * _channel;
		unsigned char * scale_images = new unsigned char[ size ];

		float scale_x = (float)_widthNew / (float)_widthOld;
		float scale_y = (float)_heightNew / (float)_heightOld;

		unsigned char * raw_new = scale_images;

		for( uint32_t i = 0 ; i != _widthNew; ++i )
		{			
			for( uint32_t j = 0; j != _heightNew; ++j )
			{
				uint32_t i_old = (uint32_t)(((float)i)/scale_x);
				uint32_t j_old = (uint32_t)(((float)j)/scale_y);

				uint32_t index = i_old + j_old * _pitchOld;

				for( uint32_t k = 0; k != _channel; ++k )
				{
					raw_new[j * _channel + k] = _image[ index * _channel + k ];
				}
			}

			raw_new += _pitchNew;
		}

		return scale_images;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * imageLoadFromMemory( const void * _data, size_t _size, uint32_t & _width, uint32_t & _height )
	{
		int wi_frame;
		int he_frame;
		int n;
		
		unsigned char * pixels = stbi_load_from_memory( (const stbi_uc *)_data, (int)_size, &wi_frame, &he_frame, &n, 4 );

		for( int i = 0; i != wi_frame; ++i )
		{
			for( int j = 0; j != he_frame; ++j )
			{ 
				int index = i + j * wi_frame;

				unsigned char r = pixels[index * 4 + 0];
				pixels[index * 4 + 0] = pixels[index * 4 + 2];
				pixels[index * 4 + 2] = r;
			}
		}

		_width = wi_frame;
		_height = he_frame;

		return pixels;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * imageLoadFromFile( const char * _fileName, uint32_t & _width, uint32_t & _height )
	{
		FILE * pFile;
		errno_t err = fopen_s( &pFile, _fileName, "rb" );

		if( err != 0 )
		{
			return nullptr;
		}
		
		fseek( pFile, 0, SEEK_END );
		size_t size = ftell( pFile );
		rewind( pFile );

		unsigned char * data = new unsigned char[size];
		fread( data, 1, size, pFile );

		fclose( pFile );

		unsigned char * pixels = imageLoadFromMemory( data, size, _width, _height );

		delete [] data;

		return pixels;
	}
	//////////////////////////////////////////////////////////////////////////
	void imageFree( unsigned char * _free )
	{
		stbi_image_free( _free );
	}
}