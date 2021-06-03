#include "ResourceImageDefaultValidator.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ImageCodecHelper.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_allPixelsTransparency( const void * _buffer, uint32_t _size )
        {
            const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer);

            for( uint32_t i = 0; i != _size; i += 4 )
            {
                if( pixel_memory[i + 3] != 0 )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_checkColumnTransparency( const void * _buffer, uint32_t _width, uint32_t _height, uint32_t _column )
        {
            const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer);

            for( uint32_t i = 0; i != _height; ++i )
            {
                if( pixel_memory[i * _width * 4 + _column * 4 + 3] != 0 )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_checkRowTransparency( const void * _buffer, uint32_t _width, uint32_t _height, uint32_t _row )
        {
            MENGINE_UNUSED( _height );

            const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer) + _row * _width * 4;

            for( uint32_t i = 0; i != _width; ++i )
            {
                if( pixel_memory[i * 4 + 3] != 0 )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_checkRowColumnTransparency( const void * _buffer, uint32_t _width, uint32_t _height )
        {
            for( uint32_t i = 0; i != _width; ++i )
            {
                if( Detail::s_checkColumnTransparency( _buffer, _width, _height, i ) == true )
                {
                    return true;
                }
            }

            for( uint32_t j = 0; j != _height; ++j )
            {
                if( Detail::s_checkRowTransparency( _buffer, _width, _height, j ) == true )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultValidator::ResourceImageDefaultValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultValidator::~ResourceImageDefaultValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefaultValidator::_validate( const ResourceImageDefaultPtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            bool validNoExist = content->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' not exist file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid open file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        if( stream->size() == 0 )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' stream '%s:%s' codec '%s' empty"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( imageDecoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' decoder initialize failed '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        bool successful = true;

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        uint32_t limitTextureWidth = CONFIG_VALUE( "Limit", "TextureWidth", 2048U );
        uint32_t limitTextureHeight = CONFIG_VALUE( "Limit", "TextureHeight", 2048U );

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;

        if( (width > limitTextureWidth && limitTextureWidth != 0U) || (height > limitTextureHeight && limitTextureHeight != 0U) )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' invalid limit [%u:%u] texture size [%u:%u]"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , limitTextureWidth
                , limitTextureHeight
                , dataInfo->width
                , dataInfo->height
            );

            successful = false;
        }

        const mt::vec2f & size = _resource->getSize();
        const mt::vec2f & maxSize = _resource->getMaxSize();

        mt::vec2f test_size;
        if( size.x < 1.f || size.y < 1.f )
        {
            test_size.x = maxSize.x;
            test_size.y = maxSize.y;
        }
        else
        {
            test_size.x = size.x;
            test_size.y = size.y;
        }

        if( (test_size.x != width || test_size.y != height) && (test_size.x > 4.f && test_size.y > 4.f) )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' incorrect size [%f:%f] texture [%u:%u]"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , test_size.x
                , test_size.y
                , width
                , height
            );

            successful = false;
        }

        bool check_imageTransparency = CONFIG_VALUE( "Check", "ImageTransparency", false );

        uint32_t dataChannels = Helper::getPixelFormatChannels( dataInfo->format );

        if( check_imageTransparency == true && dataChannels == 4 )
        {
            uint32_t texture_size = Helper::getImageCodecDataSize( dataInfo );

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( texture_size, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            void * buffer_memory = buffer->getBuffer();

            ImageDecoderData data;
            data.buffer = buffer_memory;
            data.size = texture_size;
            data.pitch = dataInfo->width * 4;
            data.format = PF_A8R8G8B8;            

            if( imageDecoder->decode( &data ) == 0 )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decode '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , content->getFileGroup()->getName().c_str()
                    , content->getFilePath().c_str()
                    , content->getCodecType().c_str()
                );

                return false;
            }

            if( Detail::s_allPixelsTransparency( buffer_memory, texture_size ) == true )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' codec '%s' all pixels transparency!"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , content->getFileGroup()->getName().c_str()
                    , content->getFilePath().c_str()
                    , content->getCodecType().c_str()
                );

                successful = false;
            }

            bool check_imageRowColumnTransparency = CONFIG_VALUE( "Check", "ImageRowColumnTransparency", false );

            if( check_imageRowColumnTransparency == true )
            {
                if( Detail::s_checkRowColumnTransparency( buffer_memory, dataInfo->width, dataInfo->height ) == true )
                {
                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' codec '%s' row or column pixels transparency!"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , content->getFileGroup()->getName().c_str()
                        , content->getFilePath().c_str()
                        , content->getCodecType().c_str()
                    );

                    successful = false;
                }
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
