#include "ResourceImageDefaultValidator.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultValidator::ResourceImageDefaultValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultValidator::~ResourceImageDefaultValidator()
    {
    }
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
            if( s_checkColumnTransparency( _buffer, _width, _height, i ) == true )
            {
                return true;
            }
        }

        for( uint32_t j = 0; j != _height; ++j )
        {
            if( s_checkRowTransparency( _buffer, _width, _height, j ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefaultValidator::_validate( const ResourceImageDefaultPtr & _resource )
    {
        const ResourceImageDefaultPtr & resource = stdex::intrusive_static_cast<ResourceImageDefaultPtr>(_resource);

        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            bool validNoExist = resource->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_ERROR( "resource '%s' group '%s' not exist file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' invalid open file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = _resource->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , _resource->getCodecType().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' decoder initialize failed '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , _resource->getCodecType().c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        uint32_t limitTextureWidth = CONFIG_VALUE( "Limit", "TextureWidth", 2048U );
        uint32_t limitTextureHeight = CONFIG_VALUE( "Limit", "TextureHeight", 2048U );

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;

        if( (width > limitTextureWidth && limitTextureWidth != 0U) || (height > limitTextureHeight && limitTextureHeight != 0U) )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid limit '%d:%d' texture size '%d:%d'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , limitTextureWidth
                , limitTextureHeight
                , dataInfo->width
                , dataInfo->height
            );

            return false;
        }

        const mt::vec2f & size = resource->getSize();
        const mt::vec2f & maxSize = resource->getMaxSize();

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
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' incorrect size %f:%f texture %d:%d"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , test_size.x
                , test_size.y
                , width
                , height
            );

            return false;
        }

        bool check_imageTransparency = CONFIG_VALUE( "Check", "ImageTransparency", false );

        if( check_imageTransparency == true && dataInfo->channels == 4 )
        {
            uint32_t texture_size = dataInfo->getSize();

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( texture_size, "ResourceImageDefault", __FILE__, __LINE__ );

            if( buffer == nullptr )
            {
                return false;
            }

            void * buffer_memory = buffer->getBuffer();

            ImageCodecOptions options;
            options.channels = 4;
            options.pitch = dataInfo->width * options.channels;

            if( imageDecoder->setOptions( &options ) == false )
            {
                LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid optionizing"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , _resource->getFileGroup()->getName().c_str()
                    , _resource->getFilePath().c_str()
                    , _resource->getCodecType().c_str()
                );

                return false;
            }

            if( imageDecoder->decode( buffer_memory, texture_size ) == 0 )
            {
                LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decode '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , _resource->getFileGroup()->getName().c_str()
                    , _resource->getFilePath().c_str()
                    , _resource->getCodecType().c_str()
                );

                return false;
            }

            if( s_allPixelsTransparency( buffer_memory, texture_size ) == true )
            {
                LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' codec '%s' all pixels transparency!"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , _resource->getFileGroup()->getName().c_str()
                    , _resource->getFilePath().c_str()
                    , _resource->getCodecType().c_str()
                );

                return false;
            }

            bool check_imageRowColumnTransparency = CONFIG_VALUE( "Check", "ImageRowColumnTransparency", false );

            if( check_imageRowColumnTransparency == true )
            {
                if( s_checkRowColumnTransparency( buffer_memory, dataInfo->width, dataInfo->height ) == true )
                {
                    LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' codec '%s' row or column pixels transparency!"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , _resource->getFileGroup()->getName().c_str()
                        , _resource->getFilePath().c_str()
                        , _resource->getCodecType().c_str()
                    );

                    return false;
                }
            }
        }

        return true;
    }
}