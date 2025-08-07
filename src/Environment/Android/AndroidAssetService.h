#pragma once

#include "Interface/AndroidAssetServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidAssetService
        : public ServiceBase<AndroidAssetServiceInterface>
    {
    public:
        AndroidAssetService();
        ~AndroidAssetService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        AAsset * open( const Char * _path, int _mode ) const override;
        void close( AAsset * _asset ) const override;

    public:
        bool existFile( const Char * _path ) const override;

    public:
        size_t read( AAsset * _asset, void * _buffer, size_t _size ) const override;
        int64_t size( AAsset * _asset ) const override;
        int64_t seek( AAsset * _asset, int64_t _offset, int _whence ) const override;

    protected:
        jobject m_jAssetManagerGlobalRef;

        AAssetManager * m_assetManager;
    };
}