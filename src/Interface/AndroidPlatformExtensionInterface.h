#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/String.h"

namespace Mengine
{
    class AndroidPlatformExtensionInterface
        : virtual public UnknownInterface
    {
    public:
        virtual JNIEnv * getJENV() const = 0;

        virtual jclass getJClassActivity() const = 0;
        virtual jobject getJObjectActivity() const = 0;

        virtual String getAndroidId() const = 0;

    public:
        virtual int32_t androidOpenAssetFile( const Char * _path ) = 0;
        virtual int32_t androidAvailableAssetFile( int32_t _fileId ) = 0;
        virtual int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) = 0;
        virtual int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) = 0;
        virtual void androidResetAssetFile( int32_t _fileId ) = 0;
        virtual void androidCloseAssetFile( int32_t _fileId ) = 0;
    };
}