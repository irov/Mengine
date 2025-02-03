#include "ResourceSoundValidator.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ContentHelper.h"

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

        bool streamable = _resource->isStreamable();

        InputStreamInterfacePtr stream = content->openInputStreamFile( streamable, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't open sound file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( decoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't create sound decoder for file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't initialize sound decoder for file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );

            return false;
        }

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        float Limit_MinimalStreamSoundDuration = CONFIG_VALUE_FLOAT( "Limit", "MinimalStreamSoundDuration", 2000.f ); //4kb

        if( (dataInfo->length <= Limit_MinimalStreamSoundDuration && Limit_MinimalStreamSoundDuration != 0.f) && streamable == true )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s'\nAdd <IsStreamable Value=\"0\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , Limit_MinimalStreamSoundDuration
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );

            return false;
        }

        float limitNoStreamSoundDurationWarning = CONFIG_VALUE_FLOAT( "Limit", "NoStreamSoundDurationWarning", 2000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationWarning && limitNoStreamSoundDurationWarning != 0.f) && streamable == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationWarning
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );
        }

        float Limit_NoStreamSoundDurationError = CONFIG_VALUE_FLOAT( "Limit", "NoStreamSoundDurationError", 10000.f ); //4kb

        if( (dataInfo->length > Limit_NoStreamSoundDurationError && Limit_NoStreamSoundDurationError != 0.f) && streamable == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s'\nAdd <IsStreamable Value=\"1\"/>"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , dataInfo->length
                , Limit_NoStreamSoundDurationError
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
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
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        soundBuffer->acquireSoundBuffer();
        soundBuffer->releaseSoundBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}