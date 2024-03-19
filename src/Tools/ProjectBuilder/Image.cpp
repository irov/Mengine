#include "Image.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ImageCodecHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const FactoryInterfacePtr & getFactoryImage()
    {
        static FactoryInterfacePtr factoryLoader = Helper::makeFactoryDefault<Image>( nullptr );

        return factoryLoader;
    }
    //////////////////////////////////////////////////////////////////////////
    ImagePtr newImage()
    {
        const FactoryInterfacePtr & factory = getFactoryImage();

        ImagePtr image = factory->createObject( MENGINE_DOCUMENT_FUNCTION );

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    Image::Image()
        : m_width( 0 )
        , m_height( 0 )
        , m_format( PF_UNKNOWN )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Image::~Image()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::load( const FilePath & _path )
    {
        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( globalFileGroup, _path, false, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            return false;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _path );

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( imageDecoder == nullptr )
        {
            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            return false;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        m_path = _path;

        m_width = dataInfo->width;
        m_height = dataInfo->height;
        m_format = dataInfo->format;

        m_memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( m_memory == nullptr )
        {
            return false;
        }

        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        void * memory_buffer = m_memory->newBuffer( m_width * m_height * channels );

        if( memory_buffer == nullptr )
        {
            return false;
        }

        uint32_t texture_size = Helper::getImageCodecDataSize( dataInfo );

        ImageDecoderData data;
        data.buffer = memory_buffer;
        data.size = texture_size;
        data.format = m_format;
        data.pitch = m_width * channels;

        if( imageDecoder->decode( &data ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::save( const FilePath & _path )
    {
        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( globalFileGroup, _path, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            return false;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _path );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( encoder == nullptr )
        {
            return false;
        }

        if( encoder->initialize( stream ) == false )
        {
            return false;
        }

        ImageCodecDataInfo info;
        info.mipmaps = 0;
        info.width = m_width;
        info.height = m_height;
        info.format = m_format;
        info.quality = 100;

        void * memory_buffer = m_memory->getBuffer();
        size_t memory_size = m_memory->getSize();

        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        ImageEncoderData data;
        data.buffer = memory_buffer;
        data.size = memory_size;
        data.pitch = m_width * channels;

        if( encoder->encode( &data, &info ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::create( uint32_t _width, uint32_t _height, uint32_t _channels )
    {
        m_width = _width;
        m_height = _height;

        switch( _channels )
        {
        case 1:
            {
                m_format = PF_L8;
            }break;
        case 3:
            {
                m_format = PF_R8G8B8;
            }break;
        case 4:
            {
                m_format = PF_A8R8G8B8;
            }break;
        }

        m_memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( m_memory == nullptr )
        {
            return false;
        }

        void * memory_buffer = m_memory->newBuffer( m_width * m_height * _channels );

        if( memory_buffer == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Image::fill( const Color & _colour )
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        uint8_t * memory = this->getMemory();

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t index = (i + (j * m_width)) * channels;

                if( channels == 4 )
                {
                    memory[index + 0] = static_cast<uint8_t>(_colour.getR() * 255.f);
                    memory[index + 1] = static_cast<uint8_t>(_colour.getG() * 255.f);
                    memory[index + 2] = static_cast<uint8_t>(_colour.getB() * 255.f);
                    memory[index + 3] = static_cast<uint8_t>(_colour.getA() * 255.f);
                }
                else if( channels == 3 )
                {
                    memory[index + 0] = static_cast<uint8_t>(_colour.getR() * 255.f);
                    memory[index + 1] = static_cast<uint8_t>(_colour.getG() * 255.f);
                    memory[index + 2] = static_cast<uint8_t>(_colour.getB() * 255.f);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::paste( const ImagePtr & _image, uint32_t _x, uint32_t _y )
    {
        uint32_t paste_width = _image->getWidth();
        uint32_t paste_height = _image->getHeight();
        uint32_t paste_channels = _image->getChannels();

        uint32_t offset_end_x = _x + _image->getWidth();
        uint32_t offset_end_y = _y + _image->getHeight();

        if( offset_end_x > m_width || offset_end_y > m_height )
        {
            return false;
        }

        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        uint8_t * this_memory = this->getMemory();
        uint8_t * paste_memory = _image->getMemory();

        for( uint32_t j = 0; j != paste_height; ++j )
        {
            for( uint32_t i = 0; i != paste_width; ++i )
            {
                uint32_t paste_index = (i + (j * paste_width)) * paste_channels;

                uint32_t to_index = ((i + _x) + ((j + _y) * m_width)) * channels;

                this_memory[to_index + 0] = paste_memory[paste_index + 0];
                this_memory[to_index + 1] = paste_memory[paste_index + 1];
                this_memory[to_index + 2] = paste_memory[paste_index + 2];

                if( paste_channels == 4 )
                {
                    this_memory[to_index + 3] = paste_memory[paste_index + 3];
                }
                else if( channels == 4 )
                {
                    this_memory[to_index + 3] = 255;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t * Image::getMemory() const
    {
        return m_memory->getBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Image::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Image::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Image::getChannels() const
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        return channels;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list Image::getdata() const
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        uint8_t * memory = this->getMemory();

        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::list pixels( kernel, m_width * m_height );

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t pixel_index = i + (j * m_width);
                uint32_t index = pixel_index * channels;

                pybind::list pixel( kernel, channels );

                for( uint32_t k = 0; k != channels; ++k )
                {
                    uint8_t color = memory[index + k];

                    pixel[k] = color;
                }

                pixels[pixel_index] = pixel;
            }
        }

        return pixels;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::putdata( const pybind::list & _data )
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        uint8_t * memory = this->getMemory();

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t pixel_index = i + (j * m_width);
                uint32_t index = pixel_index * channels;

                pybind::list pixel = _data[pixel_index];

                for( uint32_t k = 0; k != channels; ++k )
                {
                    uint8_t color = pixel[k];

                    memory[index + k] = color;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ImagePtr Image::rotate( float _angle )
    {
        MENGINE_UNUSED( _angle );

        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        ImagePtr image = newImage();

        image->create( m_height, m_width, channels );

        uint8_t * this_memory = this->getMemory();
        uint8_t * rotate_memory = image->getMemory();

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t index = (i + (j * m_width)) * channels;
                uint32_t rotate_index = ((m_height - j - 1) + (i * m_height)) * channels;

                for( uint32_t k = 0; k != channels; ++k )
                {
                    rotate_memory[rotate_index + k] = this_memory[index + k];
                }
            }
        }

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list Image::getextrema() const
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        uint8_t min_color[4];
        uint8_t max_color[4];

        for( uint32_t k = 0; k != channels; ++k )
        {
            min_color[k] = 255;
            max_color[k] = 0;
        }

        uint8_t * memory_buffer = m_memory->getBuffer();

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t index = (i + (j * m_width)) * channels;

                for( uint32_t k = 0; k != channels; ++k )
                {
                    uint8_t color = memory_buffer[index + k];

                    if( min_color[k] > color )
                    {
                        min_color[k] = color;
                    }

                    if( max_color[k] < color )
                    {
                        max_color[k] = color;
                    }
                }
            }
        }

        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::list py_extrema( kernel, channels );

        for( uint32_t k = 0; k != channels; ++k )
        {
            py_extrema[k] = pybind::make_tuple_t( kernel, min_color[k], max_color[k] );
        }

        return py_extrema;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Image::uselessalpha() const
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        if( channels != 4 )
        {
            return true;
        }

        uint8_t * memory_buffer = m_memory->getBuffer();

        uint8_t min_alpha = 255;

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t index = (i + (j * m_width)) * 4;

                uint8_t color = memory_buffer[index + 3];

                if( min_alpha > color )
                {
                    min_alpha = color;
                }
            }
        }

        if( min_alpha != 255 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::tuple Image::split() const
    {
        uint32_t channels = Helper::getPixelFormatChannels( m_format );

        ImagePtr imageRGB = newImage();
        imageRGB->create( m_width, m_height, 3 );

        ImagePtr imageAlpha = newImage();
        imageAlpha->create( m_width, m_height, 1 );

        uint8_t * memory_buffer_this = m_memory->getBuffer();
        uint8_t * memory_buffer_rgb = imageRGB->getMemory();
        uint8_t * memory_buffer_alpha = imageAlpha->getMemory();

        for( uint32_t j = 0; j != m_height; ++j )
        {
            for( uint32_t i = 0; i != m_width; ++i )
            {
                uint32_t index = (i + (j * m_width)) * channels;
                uint32_t index_rgb = (i + (j * m_width)) * 3;
                uint32_t index_a = (i + (j * m_width)) * 1;

                memory_buffer_rgb[index_rgb + 0] = memory_buffer_this[index + 0];
                memory_buffer_rgb[index_rgb + 1] = memory_buffer_this[index + 1];
                memory_buffer_rgb[index_rgb + 2] = memory_buffer_this[index + 2];

                memory_buffer_alpha[index_a + 0] = memory_buffer_this[index + 3];
            }
        }

        pybind::kernel_interface * kernel = pybind::get_kernel();

        return pybind::make_tuple_t( kernel, imageRGB, imageAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Image::release()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Image::embedding( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::interface_<Image>( _kernel, "Image", true, _module )
            .def_smart_pointer()
            .def( "getWidth", &Image::getWidth )
            .def( "getHeight", &Image::getHeight )
            .def( "getChannels", &Image::getChannels )
            .def( "paste", &Image::paste )
            .def( "getdata", &Image::getdata )
            .def( "putdata", &Image::putdata )
            .def( "rotate", &Image::rotate )
            .def( "getextrema", &Image::getextrema )
            .def( "uselessalpha", &Image::uselessalpha )
            .def( "split", &Image::split )
            .def( "release", &Image::release )
            ;
    }
}