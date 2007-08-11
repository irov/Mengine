//#	include "converter.h"
//#	include <assert.h>
#include "stdafx.h"

namespace mnglib
{

	unsigned char*	mngWriter::buffer = 0;
	unsigned long	mngWriter::sizebuf = 0;

	void	mngWriter::flush_buffer(png_structp write_png_ptr)
	{
		assert(0);
	}

	void	mngWriter::write_to_buffer(png_structp write_png_ptr, png_bytep data, png_size_t len)
	{
		char* buff = (char*)png_get_io_ptr(write_png_ptr);
		memcpy(buff+sizebuf,(char*)data,len);
		sizebuf+=len;
	}

	bool	mngWriter::createMNG(const std::string& _filename, int size)
	{
		m_seeks.clear();

		int num = 0;

		errno_t err;

		if((err = fopen_s(&m_fp,_filename.c_str(),"wb")) !=0)
		{
			return	false;
		}

		fwrite(&num, sizeof(unsigned int), 1, m_fp);
		fwrite(&num, sizeof(unsigned int), 1, m_fp);
		fwrite(&size, sizeof(unsigned int), 1, m_fp);

		for(int i = 0; i < size; i++)
		{
			int pos = ftell(m_fp);
			fwrite(&num, sizeof(unsigned int), 1, m_fp);
			m_seeks.push_back(pos);
		}

		m_currentSeek = m_seeks.begin();

		m_posInFile = 0;

		return	true;
	}

	bool	mngWriter::writeImageFromFile(const std::string& _filename)
	{
		/*	Read PNG from file	*/
		FILE* readFile = NULL;
		
		errno_t err;

		if((err = fopen_s(&readFile, _filename.c_str(), "rb")) !=0)
		{
			return	false;
		}

		char buf[4];

		if (fread(buf, 1, 4, readFile) != 4)
		{
			return false;
		}

		if (png_sig_cmp((png_bytep)buf, (png_size_t)0, 4))
		{
			return	false;
		}

		png_structp read_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (read_png_ptr == NULL)
		{
			fclose(readFile);
			readFile = NULL;
			return false;
		}

		png_infop read_info_ptr = png_create_info_struct(read_png_ptr);

		if (read_info_ptr == NULL)
		{
			png_destroy_read_struct(&read_png_ptr, png_infopp_NULL, png_infopp_NULL);     
			fclose(readFile);
			readFile = NULL;
			return false;
		}

		if (setjmp(png_jmpbuf(read_png_ptr)))
		{
			png_destroy_read_struct(&read_png_ptr, &read_info_ptr, png_infopp_NULL);
			fclose(readFile);
			readFile = NULL;
			return false;
		}

		png_init_io(read_png_ptr, readFile);
		png_set_sig_bytes(read_png_ptr, 4);
		png_read_png(read_png_ptr, read_info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

		if(read_info_ptr->channels != 4)
		{
			assert(read_info_ptr->channels == 4);
			return	false;
		}

		fclose(readFile);

		/*	Write	info:	width, height, num sprites.	*/
		static bool	initSpriteProperties = true;

		if(initSpriteProperties)
		{
			initSpriteProperties = false;

			int oldpos = ftell(m_fp);
			/*	Going	to	BEGIN OF FILE */
			/* writes OFFSET(X,Y) and Num Images */
			fseek (m_fp, 0, SEEK_SET);
			fwrite(&read_info_ptr->width, sizeof(unsigned int), 1, m_fp);
			fwrite(&read_info_ptr->height, sizeof(unsigned int), 1, m_fp);
			fseek (m_fp, oldpos, SEEK_SET);
		}

		/*	Allocate memory for temp buffer. Size is bpp*h*w.	*/
		unsigned long base_temp_size = read_info_ptr->width * read_info_ptr->height * read_info_ptr->bit_depth;

		buffer = new unsigned char[base_temp_size];

		int minX = INT_MAX; 
		int minY = INT_MAX;

		int maxX = -1; 
		int maxY = -1;

		for(unsigned short i = 0; i < read_info_ptr->width; i++)
		{
			for(unsigned short j = 0; j < read_info_ptr->height; j++)
			{
				assert(!(j >= read_info_ptr->height || i >= read_info_ptr->width));
				if(read_info_ptr->row_pointers[j][read_info_ptr->channels*i+3] & 0x000000FF)
				{
					if (i < minX)	minX = i;
					if (j < minY)	minY = j;
					if (i > maxX)	maxX = i;
					if (j > maxY)	maxY = j;
				}
			}
		}
		
		if (minX == maxX && minY == maxY)
		{
			assert(!(minX == maxX && minY == maxY));
			return	false;
		}

		int		width = maxX - minX;
		int		height = maxY - minY;
		
		sizebuf = 0;
		
		png_structp		write_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!write_png_ptr) 
		{
			return NULL; 
		}
		
		png_infop		write_info_ptr = png_create_info_struct(write_png_ptr);

		if (!write_info_ptr) 
		{
			png_destroy_write_struct(&write_png_ptr, NULL);
			return NULL; 
		}
		
		if (setjmp(write_png_ptr->jmpbuf)) 
		{
			png_destroy_write_struct(&write_png_ptr, &write_info_ptr);
			return NULL;
		}
		
		png_set_write_fn(write_png_ptr, buffer, write_to_buffer, flush_buffer);
		
		png_set_IHDR(write_png_ptr, write_info_ptr, 
			width, 
			height,
			read_info_ptr->bit_depth, 
			PNG_COLOR_TYPE_RGB_ALPHA, 
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);
		
		png_write_info(write_png_ptr, write_info_ptr);

		png_set_packing(write_png_ptr);
		
		png_bytepp row_pointers = (png_bytepp)(png_malloc(write_png_ptr, height * sizeof(png_bytep)));

		for (int j = 0; j < height; j++)
		{
			row_pointers[j]=(png_bytep)png_malloc(write_png_ptr,width*(write_info_ptr->pixel_depth/write_info_ptr->bit_depth));
			for (int i = 0; i < width; i++)
			{
				int offX = minX+i;
				int offY = minY+j;
				row_pointers[j][read_info_ptr->channels*i+0] = read_info_ptr->row_pointers[offY][read_info_ptr->channels*offX+0];
				row_pointers[j][read_info_ptr->channels*i+1] = read_info_ptr->row_pointers[offY][read_info_ptr->channels*offX+1];
				row_pointers[j][read_info_ptr->channels*i+2] = read_info_ptr->row_pointers[offY][read_info_ptr->channels*offX+2];
				row_pointers[j][read_info_ptr->channels*i+3] = read_info_ptr->row_pointers[offY][read_info_ptr->channels*offX+3];
			}
		}
		
		png_write_image(write_png_ptr, row_pointers);
		png_write_end(write_png_ptr, write_info_ptr);

		for (int i = 0; i < height; i++)
		{
			png_free(write_png_ptr,row_pointers[i]);
		}

		png_free(write_png_ptr,row_pointers);

		png_destroy_write_struct(&write_png_ptr, &write_info_ptr);

		if(sizebuf > base_temp_size)
		{
			assert(base_temp_size >= sizebuf);
			return	false;
		}

		int seek = ftell(m_fp);
		
		int posBegin = *m_currentSeek;

		fseek (m_fp, posBegin, SEEK_SET);
		fwrite(&seek, sizeof(unsigned int), 1, m_fp);

		fseek (m_fp, seek, SEEK_SET);	

		m_currentSeek++;

		fwrite(&minX, sizeof(unsigned int), 1, m_fp);
		fwrite(&minY, sizeof(unsigned int), 1, m_fp);
		fwrite(&sizebuf, sizeof(unsigned int), 1, m_fp);
		fwrite(buffer, sizeof(char), sizebuf, m_fp);

		delete	buffer;

		png_destroy_read_struct(&read_png_ptr, &read_info_ptr, png_infopp_NULL);
		return	true; 
	}
 
	bool	mngWriter::closeMNG()
	{
		fclose(m_fp);
		return	true;
	}
};