#include "ToolPNG.h"
#include "ToolUtils.h"

#include "png.h"

#include <malloc.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
bool is_png_buffer( const uint8_t * _buffer, size_t _size )
{
    if( _size < 8 )
    {
        return false;
    }

    if( png_sig_cmp( (const png_bytep)_buffer, 0, _size ) != 0 )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool is_png_file( const wchar_t * _path )
{
    uint8_t buffer[8];
    size_t size;
    if( read_file_to_memory( _path, buffer, 8, &size ) == false )
    {
        return false;
    }

    if( size != 8 )
    {
        return false;
    }

    if( is_png_buffer( buffer, 8 ) == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
uint8_t * png_load_from_memory( const uint8_t * _buffer, size_t _size, uint32_t * const _width, uint32_t * const _height, uint32_t * const _channels )
{
    if( _buffer == nullptr || _size == 0 )
    {
        return nullptr;
    }

    png_image image;
    ::memset( &image, 0, sizeof( image ) );
    image.version = PNG_IMAGE_VERSION;

    if( png_image_begin_read_from_memory( &image, _buffer, _size ) == 0 )
    {
        return nullptr;
    }

    image.format = PNG_FORMAT_RGBA;
    uint8_t * pixels = (uint8_t *)::malloc( PNG_IMAGE_SIZE( image ) );

    if( pixels == nullptr )
    {
        png_image_free( &image );

        return nullptr;
    }

    if( png_image_finish_read( &image, NULL, pixels, 0, NULL ) != 0 )
    {
        ::free( pixels );
        png_image_free( &image );

        return nullptr;
    }

    *_width = image.width;
    *_height = image.height;
    *_channels = PNG_IMAGE_PIXEL_COMPONENT_SIZE( image.format );

    png_image_free( &image );

    return pixels;
}
//////////////////////////////////////////////////////////////////////////
const uint8_t * png_write_to_memory( const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels, size_t * const _write )
{
    if( _buffer == nullptr || _width == 0 || _height == 0 )
    {
        return nullptr;
    }

    png_image image;
    ::memset( &image, 0, sizeof( image ) );
    image.version = PNG_IMAGE_VERSION;
    image.width = _width;
    image.height = _height;

    if( _channels == 3 )
    {
        image.format = PNG_FORMAT_RGB;
    }
    else if( _channels == 4 )
    {
        image.format = PNG_FORMAT_RGBA;
    }
    else
    {
        return nullptr;
    }

    png_alloc_size_t needed = 0;
    if( png_image_write_to_memory( &image, nullptr, &needed, 0, _buffer, _size, nullptr ) == 0 )
    {
        png_image_free( &image );

        return nullptr;
    }

    uint8_t * buffer = (uint8_t *)::malloc( needed );

    if( buffer == nullptr )
    {
        png_image_free( &image );

        return nullptr;
    }

    if( png_image_write_to_memory( &image, buffer, &needed, 0, _buffer, static_cast<png_int_32>(_size), nullptr ) == 0 )
    {
        ::free( buffer );
        png_image_free( &image );

        return nullptr;
    }

    *_write = needed;

    return buffer;
}
//////////////////////////////////////////////////////////////////////////
const uint8_t * png_move_to_memory( const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels, size_t * const _write )
{
    const uint8_t * mem = png_write_to_memory( _buffer, _size, _width, _height, _channels, _write );

    ::free( (void *)_buffer );

    return mem;
}
//////////////////////////////////////////////////////////////////////////
uint8_t * png_load_from_file( const wchar_t * _path, uint32_t * const _width, uint32_t * const _height, uint32_t * const _channels )
{
    uint8_t * f_in_memory;
    size_t f_in_size;
    if( read_file_memory( _path, &f_in_memory, &f_in_size ) == false )
    {
        return nullptr;
    }

    uint8_t * data_in = png_load_from_memory( f_in_memory, f_in_size, _width, _height, _channels );

    ::free( f_in_memory );

    return data_in;
}
//////////////////////////////////////////////////////////////////////////
bool png_move_to_file( const wchar_t * _path, const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels )
{
    size_t len_out;
    const uint8_t * memory_out = png_move_to_memory( _buffer, _size, _width, _height, _channels, &len_out );

    if( memory_out == NULL )
    {
        return false;
    }

    if( write_file_memory( _path, memory_out, len_out ) == false )
    {
        ::free( (void *)memory_out );

        return false;
    }
    
    ::free( (void *)memory_out );

    return true;
}
//////////////////////////////////////////////////////////////////////////