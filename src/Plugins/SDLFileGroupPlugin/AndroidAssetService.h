#pragma once

#include "AndroidAssetServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidAssetService
        : public ServiceBase<AndroidAssetServiceInterface>
    {
    public:
        AndroidAssetService();
        ~AndroidAssetService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool existFile( const Char * _filePath ) const override;

    protected:
        jobject m_pAssetManagerJava;
        AAssetManager * m_pAssetManager;
    };
}