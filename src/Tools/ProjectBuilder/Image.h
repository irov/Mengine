#pragma once

#include "Interface/ImageCodecInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Color.h"
#include "Kernel/FilePath.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class Image
    {
    public:
        Image();
        ~Image();

    public:
        bool load( const FilePath & _path );
        bool save( const FilePath & _path );

        bool create( uint32_t _width, uint32_t _height, uint32_t _channels );

        void fill( const Color & _colour );

        bool paste( Image * _image, uint32_t _x, uint32_t _y );

        Image * rotate( float _angle );

        void getExtremColor( uint8_t min_color[4], uint8_t max_color[4] ) const;
        bool uselessalpha() const;

        void split( Image ** _imageRGB, Image ** _imageAlpha ) const;

        void release();

    public:
        uint8_t * getMemory() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getChannels() const;

    protected:
        FilePath m_path;

        MemoryBufferInterfacePtr m_memory;

        uint32_t m_width;
        uint32_t m_height;

        EPixelFormat m_format;
    };
}