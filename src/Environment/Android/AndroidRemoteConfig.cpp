#include "AndroidRemoteConfig.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidFragmentHelper.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Resolution.h"
#include "Kernel/TagsHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Char * ANDROID_REMOTE_CONFIG_NAME = "global";
    //////////////////////////////////////////////////////////////////////////
    AndroidRemoteConfig::AndroidRemoteConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidRemoteConfig::~AndroidRemoteConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & AndroidRemoteConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::unload()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::existValue( const Char * _section, const Char * _key ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jboolean jresult = Helper::AndroidCallBooleanFragmentMethod( jenv, "MengineFragmentRemoteConfig", "hasRemoteConfigSectionKey", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == JNI_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueBoolean", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Boolean;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return false;
        }

        jclass jclass_Boolean = Mengine_JNI_GetClassBoolean( jenv );

        jboolean jboolean_value = Helper::AndroidGetJavaObjectValueBoolean( jenv, jclass_Boolean, jresult );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_Boolean );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = (bool)jboolean_value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueLong", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Long;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return false;
        }

        jclass jclass_Long = Mengine_JNI_GetClassLong( jenv );

        jlong jlong_value = Helper::AndroidGetJavaObjectValueLong( jenv, jclass_Long, jresult );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_Long );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = (int64_t)jlong_value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueDouble", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Double;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return false;
        }

        jclass jclass_Double = Mengine_JNI_GetClassDouble( jenv );

        jdouble jdouble_value = Helper::AndroidGetJavaObjectValueDouble( jenv, jclass_Double, jresult );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_Double );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = (double)jdouble_value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const String & _default, String * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return false;
        }

        jstring jstring_result = (jstring)jresult;

        *_value = Helper::AndroidMakeStringFromJString( jenv, jstring_result );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_result );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return false;
        }

        jstring jstring_result = (jstring)jresult;

        ConstString result = Helper::AndroidMakeConstStringFromJString( jenv, jstring_result );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_result );

        *_value = result;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        MENGINE_UNUSED( _section );

        ConstString result_string;
        if( this->hasValue( _section, _key, ConstString::none(), &result_string ) == false )
        {
            *_value = _default;

            return false;
        }

        FilePath result = Helper::stringizeFilePath( result_string );

        *_value = result;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            *_value = _default;

            return false;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        Tags result;
        Helper::AndroidForeachJavaJSONArray( jenv, jclass_JSONArray, jresult, [jenv, &result]( jint _index, jobject _jvalue )
        {
            MENGINE_UNUSED( _index );

            jstring jstring_tag = (jstring)_jvalue;

            ConstString tag = Helper::AndroidMakeConstStringFromJString( jenv, jstring_tag );

            result.addTag( tag );
        } );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = result;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;

            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            *_value = _default;

            return false;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        uint32_t width = (uint32_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 0, 0 );
        uint32_t height = (uint32_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 1, 0 );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = Resolution( width, height );;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            *_value = _default;
            return false;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            *_value = _default;
            return false;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        uint8_t r = (uint8_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 0, 0 );
        uint8_t g = (uint8_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 1, 0 );
        uint8_t b = (uint8_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 2, 0 );
        uint8_t a = (uint8_t)Helper::AndroidGetJavaJSONArrayInt( jenv, jclass_JSONArray, jresult, 3, 255 );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );

        *_value = Helper::makeColor8( r, g, b, a );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::hasSection( const Char * _section ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return false;
        }

        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );

        jboolean jresult = Helper::AndroidCallBooleanFragmentMethod( jenv, "MengineFragmentRemoteConfig", "hasRemoteConfig", "(Ljava/lang/String;)Z", jstring_section );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );

        return (jresult == JNI_TRUE);
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const String & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidRemoteConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        Helper::AndroidForeachJavaJSONArray( jenv, jclass_JSONArray, jresult, [jenv, jclass_JSONArray, _values]( jint _index, jobject _jvalue )
        {
            MENGINE_UNUSED( _index );

            if( Mengine_JNI_IsInstanceOf( jenv, _jvalue, jclass_JSONArray ) != JNI_TRUE )
            {
                return;
            }

            jobject jarray_element = _jvalue;

            jint jarray_length = Helper::AndroidGetJavaJSONArrayLength( jenv, jclass_JSONArray, jarray_element );

            if( jarray_length < 6 )
            {
                return;
            }

            AspectRatioViewport aspect;
            aspect.width = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 0, 0.f );
            aspect.height = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 1, 0.f );
            aspect.viewport.begin.x = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 2, 0.f );
            aspect.viewport.begin.y = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 3, 0.f );
            aspect.viewport.end.x = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 4, 0.f );
            aspect.viewport.end.y = (float)Helper::AndroidGetJavaJSONArrayFloat( jenv, jclass_JSONArray, jarray_element, 5, 0.f );

            _values->emplace_back( aspect );
        } );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        Helper::AndroidForeachJavaJSONArray( jenv, jclass_JSONArray, jresult, [jenv, _values]( jint _index, jobject _jvalue )
        {
            MENGINE_UNUSED( _index );

            jstring jstring_element = (jstring)_jvalue;

            FilePath element = Helper::AndroidMakeFilePathFromJString( jenv, jstring_element );

            _values->emplace_back( element );
        } );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        Helper::AndroidForeachJavaJSONArray( jenv, jclass_JSONArray, jresult, [jenv, _values]( jint _index, jobject _jvalue )
        {
            MENGINE_UNUSED( _index );

            jstring jstring_element = (jstring)_jvalue;

            ConstString element = Helper::AndroidMakeConstStringFromJString( jenv, jstring_element );

            _values->emplace_back( element );
        } );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidRemoteConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        jstring jstring_name = Helper::AndroidMakeJObjectString( jenv, ANDROID_REMOTE_CONFIG_NAME );
        jstring jstring_section = Helper::AndroidMakeJObjectString( jenv, _section );
        jstring jstring_key = Helper::AndroidMakeJObjectString( jenv, _key );

        jobject jresult = Helper::AndroidCallObjectFragmentMethod( jenv, "MengineFragmentRemoteConfig", "getRemoteConfigSectionValueJSONArray", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/json/JSONArray;", jstring_name, jstring_section, jstring_key );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_name );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_section );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_key );

        if( jresult == nullptr )
        {
            return;
        }

        jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( jenv );

        Helper::AndroidForeachJavaJSONArray( jenv, jclass_JSONArray, jresult, [jenv, _values]( jint _index, jobject _jvalue )
        {
            MENGINE_UNUSED( _index );

            jstring jstring_element = (jstring)_jvalue;

            String element = Helper::AndroidMakeStringFromJString( jenv, jstring_element );

            _values->emplace_back( element );
        } );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_JSONArray );
        Mengine_JNI_DeleteLocalRef( jenv, jresult );
    }
    //////////////////////////////////////////////////////////////////////////
}