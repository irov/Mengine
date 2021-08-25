#include "ResourceSoundValidator.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceSoundValidator::ResourceSoundValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSoundValidator::~ResourceSoundValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSoundValidator::_validate( const ResourceSoundPtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        bool streamable = _resource->isStreamable();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, streamable, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't open sound file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( decoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't create sound decoder for file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't initialize sound decoder for file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        float limitMinimalStreamSoundDuration = CONFIG_VALUE( "Limit", "MinimalStreamSoundDuration", 500.f ); //4kb

        if( (dataInfo->length <= limitMinimalStreamSoundDuration && limitMinimalStreamSoundDuration != 0.f) && streamable == true )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"0\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitMinimalStreamSoundDuration
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        float limitNoStreamSoundDurationWarning = CONFIG_VALUE( "Limit", "NoStreamSoundDurationWarning", 2000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationWarning && limitNoStreamSoundDurationWarning != 0.f) && streamable == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationWarning
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );
        }

        float limitNoStreamSoundDurationError = CONFIG_VALUE( "Limit", "NoStreamSoundDurationError", 10000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationError && limitNoStreamSoundDurationError != 0.f) && streamable == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationError
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        decoder = nullptr;
        stream = nullptr;

        SoundBufferInterfacePtr soundBuffer = _resource->createSoundBuffer( MENGINE_DOCUMENT_FACTORABLE );

        if( soundBuffer == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't create buffer '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        soundBuffer->acquireSoundBuffer();
        soundBuffer->releaseSoundBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}