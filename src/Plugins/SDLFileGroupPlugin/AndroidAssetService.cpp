#include "AndroidAssetService.h"

#include <jni.h>
#include "SDL_system.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidAssetService, Mengine::AndroidAssetService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidAssetService::AndroidAssetService()
        : m_pAssetManager(nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidAssetService::~AndroidAssetService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetService::_initializeService()
    {
        JNIEnv * env = (JNIEnv *)SDL_AndroidGetJNIEnv();

        jobject activity = (jobject)SDL_AndroidGetActivity();

        jclass activity_class = env->GetObjectClass( activity );

        jmethodID activity_class_getAssets = env->GetMethodID( activity_class, "getAssets", "()Landroid/content/res/AssetManager;" );
        jobject asset_manager = env->CallObjectMethod( activity, activity_class_getAssets ); // activity.getAssets();
        jobject asset_manager_java = env->NewGlobalRef( asset_manager );

        AAssetManager * pAssetManager = AAssetManager_fromJava( env, asset_manager_java );

        if( pAssetManager == nullptr )
        {
            return false;
        }

        m_pAssetManagerJava = asset_manager_java;
        m_pAssetManager = pAssetManager;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////     
    void AndroidAssetService::_finalizeService()
    {
        if( m_pAssetManagerJava != nullptr )
        {
            JNIEnv * env = (JNIEnv *)SDL_AndroidGetJNIEnv();

            env->DeleteGlobalRef( m_pAssetManagerJava );
            m_pAssetManagerJava = nullptr;
            m_pAssetManager = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetService::existFile( const Char * _filePath ) const
    {
        AAsset * asset = AAssetManager_open( m_pAssetManager, _filePath, AASSET_MODE_UNKNOWN );

        if( asset == nullptr )
        {
            return false;
        }

        AAsset_close( asset );

        return true;
    }
}

