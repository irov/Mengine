#include "AndroidAssetService.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidAssetService, Mengine::AndroidAssetService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidAssetService::AndroidAssetService()
        : m_jAssetManagerGlobalRef( nullptr )
        , m_assetManager( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidAssetService::~AndroidAssetService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetService::_initializeService()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject jAssetManager = Helper::AndroidCallObjectApplicationMethod( jenv, "getAssets", "()Landroid/content/res/AssetManager;" );

        if( jAssetManager == nullptr )
        {
            LOGGER_ERROR( "invalid get asset manager" );

            return false;
        }

        jobject jobject_AssetManagerGlobalRef = Mengine_JNI_NewGlobalRef( jenv, jAssetManager );

        AAssetManager * assetManager = Mengine_JNI_GetAssetManagerFromJava( jenv, jobject_AssetManagerGlobalRef );

        if( assetManager == nullptr )
        {
            LOGGER_ERROR( "invalid get asset manager" );

            return false;
        }

        m_jAssetManagerGlobalRef = jobject_AssetManagerGlobalRef;
        m_assetManager = assetManager;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetService::_finalizeService()
    {
        if(m_jAssetManagerGlobalRef != nullptr )
        {
            JNIEnv * jenv = Mengine_JNI_GetEnv();

            MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

            Mengine_JNI_DeleteGlobalRef( jenv, m_jAssetManagerGlobalRef );
            m_jAssetManagerGlobalRef = nullptr;
        }

        m_assetManager = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AAsset * AndroidAssetService::open( const Char * _path, int _mode ) const
    {
        AAsset * asset = AAssetManager_open( m_assetManager, _path, _mode );

        return asset;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetService::close( AAsset * _asset ) const
    {
        AAsset_close( _asset );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetService::existFile( const Char * _path ) const
    {
        AAsset * asset = AAssetManager_open( m_assetManager, _path, AASSET_MODE_UNKNOWN);

        if( asset == nullptr )
        {
            return false;
        }

        AAsset_close(asset);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidAssetService::read( AAsset * _asset, void * _buffer, size_t _size ) const
    {
        int read = AAsset_read( _asset, _buffer, _size );

        return (size_t)read;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidAssetService::size( AAsset * _asset ) const
    {
        off64_t size = AAsset_getLength64( _asset );

        return (int64_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidAssetService::seek( AAsset * _asset, int64_t _offset, int _whence ) const
    {
        off64_t seek = AAsset_seek64( _asset, _offset, _whence );

        return (int64_t)seek;
    }
    //////////////////////////////////////////////////////////////////////////
}