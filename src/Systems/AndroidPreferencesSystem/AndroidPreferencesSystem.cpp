#include "AndroidPreferencesSystem.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PreferencesSystem, Mengine::AndroidPreferencesSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidPreferencesSystem::AndroidPreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidPreferencesSystem::~AndroidPreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPreferencesSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidPreferencesSystem::getPreferenceInteger( const Char * _key, int64_t _default ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return _default;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );
        jlong jdefault = (jlong)_default;

        jlong jvalue = Helper::AndroidCallLongApplicationMethod( jenv, "getPreferenceInteger", "(Ljava/lang/String;J)J", jkey, jdefault );

        jenv->DeleteLocalRef( jkey );

        int64_t value = jvalue;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::setPreferenceInteger( const Char * _key, int64_t _value )
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );
        jlong jvalue = (jlong)_value;

        Helper::AndroidCallVoidApplicationMethod( jenv, "setPreferenceInteger", "(Ljava/lang/String;J)V", jkey, jvalue );

        jenv->DeleteLocalRef( jkey );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::getPreferenceString( const Char * _key, Char * const _value, size_t _capacity, size_t * const _size ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );

        jboolean exist = Helper::AndroidCallBooleanApplicationMethod( jenv, "hasPreference", "(Ljava/lang/String;)Z", jkey );

        if( exist == JNI_FALSE )
        {
            jenv->DeleteLocalRef( jkey );

            return false;
        }

        jstring jvalue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getPreferenceString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", jkey, nullptr );

        jenv->DeleteLocalRef( jkey );

        Helper::AndroidCopyStringFromJString(jenv, jvalue, _value, _capacity);

        jenv->DeleteLocalRef( jvalue );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::setPreferenceString( const Char * _key, const Char * _value )
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );
        jstring jvalue = jenv->NewStringUTF( _value );

        Helper::AndroidCallVoidApplicationMethod( jenv, "setPreferenceString", "(Ljava/lang/String;Ljava/lang/String;)V", jkey, jvalue );

        jenv->DeleteLocalRef( jkey );
        jenv->DeleteLocalRef( jvalue );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString AndroidPreferencesSystem::getPreferenceConstString( const Char * _key, const ConstString & _default ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return _default;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        const Char * default_str = _default.c_str();

        jstring jkey = jenv->NewStringUTF( _key );
        jstring jdefault = jenv->NewStringUTF( default_str );

        jstring jvalue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getPreferenceString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", jkey, jdefault );

        jenv->DeleteLocalRef( jkey );
        jenv->DeleteLocalRef( jdefault );

        ConstString value = Helper::AndroidMakeConstStringFromJString( jenv, jvalue );

        jenv->DeleteLocalRef( jvalue );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::setPreferenceConstString( const Char * _key, const ConstString & _value )
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        const Char * value_str = _value.c_str();

        jstring jkey = jenv->NewStringUTF( _key );
        jstring jvalue = jenv->NewStringUTF( value_str );

        Helper::AndroidCallVoidApplicationMethod( jenv, "setPreferenceString", "(Ljava/lang/String;Ljava/lang/String;)V", jkey, jvalue );

        jenv->DeleteLocalRef( jkey );
        jenv->DeleteLocalRef( jvalue );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPreferencesSystem::hasPreference( const Char * _key ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );

        jboolean exist = Helper::AndroidCallBooleanApplicationMethod( jenv, "hasPreference", "(Ljava/lang/String;)Z", jkey );

        jenv->DeleteLocalRef( jkey );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPreferencesSystem::removePreference( const Char * _key )
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jkey = jenv->NewStringUTF( _key );

        Helper::AndroidCallVoidApplicationMethod( jenv, "removePreference", "(Ljava/lang/String;)V", jkey );

        jenv->DeleteLocalRef( jkey );
    }
    //////////////////////////////////////////////////////////////////////////
}
