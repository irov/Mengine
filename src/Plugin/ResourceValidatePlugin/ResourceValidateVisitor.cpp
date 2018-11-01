#include "ResourceValidateVisitor.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PickCodecInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "ResourceValidateInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceValidateVisitor::ResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceValidateVisitor::~ResourceValidateVisitor()
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
        (void)_height;

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
    bool ResourceValidateVisitor::accept( ResourceImageDefault * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            bool validNoExist = _resource->isValidNoExist();

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
            ->openInputFile( fileGroup, filePath, false );

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
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType );

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

        float width = (float)dataInfo->width;
        float height = (float)dataInfo->height;

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
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' incorrect size %f:%f texture %f:%f"
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
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceImageData * _resource )
    {
        const FilePath & fileName = _resource->getFileName();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        bool exist = fileGroup->existFile( fileName );

        if( exist == false )
        {
            bool validNoExist = _resource->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_ERROR( "resource '%s' group '%s' not exist file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFileName().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, fileName, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' invalid open file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFileName().c_str()
            );

            return false;
        }

        const ConstString & codecType = _resource->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFileName().c_str()
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
                , _resource->getFileName().c_str()
                , _resource->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceImageSequence * _resource )
    {
        size_t total_memory = 0;

        const VectorFrameImageSequence & sequences = _resource->getSequence();

        for( const FrameImageSequence & sequence : sequences )
        {
            ResourceImagePtr resourceImage;
            if( RESOURCE_SERVICE()
                ->hasResourceT<ResourceImagePtr>( sequence.resourceName, &resourceImage ) == false )
            {
                LOGGER_ERROR( "resource '%s' group '%s' not found image resource '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , sequence.resourceName.c_str()
                );

                return false;
            }

            if( RESOURCEVALIDATE_SERVICE()
                ->validResource( resourceImage ) == false )
            {
                LOGGER_ERROR( "resource '%s' group '%s' invalid validate sequence resource '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }

            const mt::vec2f & size = resourceImage->getSize();

            uint32_t width = Helper::getTexturePOW2( (uint32_t)size.x );
            uint32_t height = Helper::getTexturePOW2( (uint32_t)size.y );

            uint32_t channels = 3;

            if( resourceImage->hasAlpha() == true )
            { 
                channels = 4;
            }

            size_t textureSize = Helper::getTextureMemorySize( width, height, channels, 1, PF_UNKNOWN );

            total_memory += textureSize;
        }

        uint32_t animationMemoryLimit = CONFIG_VALUE( "Limit", "AnimationMemoryLimit", 4194304U ); //4mb

        if( total_memory > animationMemoryLimit && animationMemoryLimit != 0U )
        {
            LOGGER_ERROR( "resource '%s' group '%s' overflow %.2fmb max video memory %.2fmb (coeff %f)"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , float( total_memory ) / (1024.f * 1024.f)
                , float( animationMemoryLimit ) / (1024.f * 1024.f)
                , float( total_memory ) / float( animationMemoryLimit )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceMusic * _resource )
    {
        bool external = _resource->isExternal();

        if( external == true )
        {
            return true;
        }

        const FilePath & filePath = _resource->getPath();

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' music '%s' not exist"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getPath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceSound * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        bool streamable = _resource->isStreamable();

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, streamable );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group %s can't open sound file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = _resource->getCodecType();

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( codecType );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't create sound decoder for file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't initialize sound decoder for file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        float limitMinimalStreamSoundDuration = CONFIG_VALUE( "Limit", "MinimalStreamSoundDuration", 500.f ); //4kb

        if( (dataInfo->length <= limitMinimalStreamSoundDuration && limitMinimalStreamSoundDuration != 0.f) && streamable == true )
        {
            LOGGER_ERROR( "resource '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"0\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitMinimalStreamSoundDuration
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        float limitNoStreamSoundDurationWarning = CONFIG_VALUE( "Limit", "NoStreamSoundDurationWarning", 2000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationWarning && limitNoStreamSoundDurationWarning != 0.f) && streamable == false )
        {
            LOGGER_WARNING( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationWarning
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );
        }

        float limitNoStreamSoundDurationError = CONFIG_VALUE( "Limit", "NoStreamSoundDurationError", 10000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationError && limitNoStreamSoundDurationError != 0.f) && streamable == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationError
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        decoder = nullptr;
        stream = nullptr;

        SoundBufferInterfacePtr buffer = _resource->createSoundBuffer();

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't create buffer '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceHIT * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            return false;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false );

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

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<PickDecoderInterfacePtr>( codecType );

        if( decoder == nullptr )
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

        if( decoder->prepareData( stream ) == false )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateVisitor::accept( ResourceFile * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file group '%s' not found file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
}