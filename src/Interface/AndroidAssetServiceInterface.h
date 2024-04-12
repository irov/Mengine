#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    class AndroidAssetServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidAssetService" )

    public:
        virtual AAsset * open( const Char * _path, int _mode ) const = 0;
        virtual void close( AAsset * _asset ) const = 0;

    public:
        virtual size_t read( AAsset * _asset, void * _buffer, size_t _size ) const = 0;
        virtual int64_t size( AAsset * _asset ) const = 0;
        virtual int64_t seek( AAsset * _asset, int64_t _offset, int _whence ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_ASSET_SERVICE()\
    ((Mengine::AndroidAssetServiceInterface *)SERVICE_GET(Mengine::AndroidAssetServiceInterface))
//////////////////////////////////////////////////////////////////////////