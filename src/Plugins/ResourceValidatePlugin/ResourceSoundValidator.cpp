#include "ResourceSoundValidator.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
    bool ResourceSoundValidator::_validate( const ResourceSoundPtr & _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        bool streamable = _resource->isStreamable();

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, streamable, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE( "resource '%s' group '%s' can't open sound file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = _resource->getCodecType();

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
        {
            LOGGER_MESSAGE( "resource '%s' group '%s' can't create sound decoder for file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE( "resource '%s' group '%s' can't initialize sound decoder for file '%s:%s'"
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
            LOGGER_MESSAGE( "resource '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"0\"/>"
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
            LOGGER_MESSAGE( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
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
            LOGGER_MESSAGE( "resource '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
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
            LOGGER_MESSAGE( "resource '%s' group '%s' can't create buffer '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
}