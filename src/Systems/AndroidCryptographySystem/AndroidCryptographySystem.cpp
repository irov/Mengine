#include "AndroidCryptographySystem.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::AndroidCryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidCryptographySystem::AndroidCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidCryptographySystem::~AndroidCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidCryptographySystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jlong jseed = Helper::AndroidCallLongApplicationMethod( jenv, "getSecureRandomNumber", "()J" );

        *_seed = (uint64_t)jseed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jhex = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getSecureRandomHexString", "(I)Ljava/lang/String;", _length );

        const Char * hex = jenv->GetStringUTFChars( jhex, nullptr );

        MENGINE_STRCPY( _hexadecimal, hex );

        jenv->ReleaseStringUTFChars( jhex, hex );
        jenv->DeleteLocalRef( jhex );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
