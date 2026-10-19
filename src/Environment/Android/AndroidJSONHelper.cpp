#include "AndroidJSONHelper.h"

#include "Environment/Android/AndroidHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            const uint32_t ANDROID_JSON_MAX_DEPTH = 256;
            //////////////////////////////////////////////////////////////////////////
            struct AndroidJSONTypesDesc
            {
                jclass booleanClass;
                jclass integerClass;
                jclass longClass;
                jclass byteClass;
                jclass shortClass;
                jclass floatClass;
                jclass doubleClass;
                jclass stringClass;
                jclass objectClass;
                jclass arrayClass;
                jobject nullValue;
            };
            //////////////////////////////////////////////////////////////////////////
            static jobject getJSONObjectValue( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, const Char * _method, const Char * _signature )
            {
                if( _jobject == nullptr )
                {
                    return nullptr;
                }

                jmethodID method = Mengine_JNI_GetMethodID( _jenv, _jclass, _method, _signature );
                jstring key = Mengine_JNI_NewStringUTF( _jenv, _key );
                jobject value = Mengine_JNI_CallObjectMethod( _jenv, _jobject, method, key );
                Mengine_JNI_DeleteLocalRef( _jenv, key );
                Helper::AndroidEnvExceptionCheck( _jenv );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool makeString( JNIEnv * _jenv, jstring _string, String * const _value )
            {
                jclass stringClass = Mengine_JNI_GetClassString( _jenv );
                jmethodID getBytes = Mengine_JNI_GetMethodID( _jenv, stringClass, "getBytes", "(Ljava/lang/String;)[B" );
                jstring encoding = Mengine_JNI_NewStringUTF( _jenv, "UTF-8" );
                jobject encoded = Mengine_JNI_CallObjectMethod( _jenv, _string, getBytes, encoding );
                jbyteArray bytes = static_cast<jbyteArray>(encoded);
                Mengine_JNI_DeleteLocalRef( _jenv, encoding );
                Mengine_JNI_DeleteLocalRef( _jenv, stringClass );
                Helper::AndroidEnvExceptionCheck( _jenv );

                if( bytes == nullptr )
                {
                    return false;
                }

                jsize length = _jenv->GetArrayLength( bytes );
                jbyte * characters = Mengine_JNI_GetByteArrayElements( _jenv, bytes, nullptr );
                if( characters == nullptr )
                {
                    Mengine_JNI_DeleteLocalRef( _jenv, bytes );

                    return false;
                }

                const Char * buffer = reinterpret_cast<const Char *>(characters);
                size_t size = static_cast<size_t>(length);
                _value->assign( buffer, size );
                Mengine_JNI_ReleaseByteArrayElements( _jenv, bytes, characters, JNI_ABORT );
                Mengine_JNI_DeleteLocalRef( _jenv, bytes );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool makeJSON( JNIEnv * _jenv, jobject _jobject, const AndroidJSONTypesDesc & _types, uint32_t _depth, jpp::object * const _json )
            {
                if( _depth >= ANDROID_JSON_MAX_DEPTH )
                {
                    return false;
                }

                if( _jobject == nullptr || _jenv->IsSameObject( _jobject, _types.nullValue ) == JNI_TRUE )
                {
                    *_json = jpp::make_null();
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.booleanClass ) == JNI_TRUE )
                {
                    jboolean value = Helper::AndroidGetJavaObjectValueBoolean( _jenv, _types.booleanClass, _jobject );
                    bool booleanValue = value == JNI_TRUE;
                    *_json = jpp::make_boolean( booleanValue );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.integerClass ) == JNI_TRUE )
                {
                    jint value = Helper::AndroidGetJavaObjectValueInteger( _jenv, _types.integerClass, _jobject );
                    *_json = jpp::make_integer( value );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.longClass ) == JNI_TRUE )
                {
                    jlong value = Helper::AndroidGetJavaObjectValueLong( _jenv, _types.longClass, _jobject );
                    *_json = jpp::make_integer( value );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.byteClass ) == JNI_TRUE || Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.shortClass ) == JNI_TRUE )
                {
                    jclass valueClass = Mengine_JNI_GetObjectClass( _jenv, _jobject );
                    jmethodID longValue = Mengine_JNI_GetMethodID( _jenv, valueClass, "longValue", "()J" );
                    Mengine_JNI_DeleteLocalRef( _jenv, valueClass );
                    jlong value = Mengine_JNI_CallLongMethod( _jenv, _jobject, longValue );
                    Helper::AndroidEnvExceptionCheck( _jenv );
                    *_json = jpp::make_integer( value );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.floatClass ) == JNI_TRUE )
                {
                    jfloat value = Helper::AndroidGetJavaObjectValueFloat( _jenv, _types.floatClass, _jobject );
                    *_json = jpp::make_real( value );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.doubleClass ) == JNI_TRUE )
                {
                    jdouble value = Helper::AndroidGetJavaObjectValueDouble( _jenv, _types.doubleClass, _jobject );
                    *_json = jpp::make_real( value );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.stringClass ) == JNI_TRUE )
                {
                    jstring string = static_cast<jstring>(_jobject);
                    String value;
                    if( Detail::makeString( _jenv, string, &value ) == false )
                    {
                        return false;
                    }

                    const Char * buffer = value.c_str();
                    size_t size = value.size();
                    *_json = jpp::make_stringn( buffer, size );
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.objectClass ) == JNI_TRUE )
                {
                    jpp::object object = jpp::make_object();
                    bool successful = true;
                    uint32_t childDepth = _depth + 1;
                    Helper::AndroidForeachJavaJSONObject( _jenv, _types.objectClass, _jobject, [_jenv, &_types, childDepth, &object, &successful]( jobject _key, jobject _value )
                    {
                        if( successful == false )
                        {
                            return;
                        }

                        jstring string = static_cast<jstring>(_key);
                        String key;
                        if( Detail::makeString( _jenv, string, &key ) == false )
                        {
                            successful = false;

                            return;
                        }

                        jpp::object value;
                        if( Detail::makeJSON( _jenv, _value, _types, childDepth, &value ) == false )
                        {
                            successful = false;

                            return;
                        }

                        const Char * keyBuffer = key.c_str();
                        size_t keySize = key.size();
                        successful = object.setn( keyBuffer, keySize, value );
                    } );

                    if( successful == false )
                    {
                        return false;
                    }

                    *_json = object;
                }
                else if( Mengine_JNI_IsInstanceOf( _jenv, _jobject, _types.arrayClass ) == JNI_TRUE )
                {
                    jmethodID lengthMethod = Mengine_JNI_GetMethodID( _jenv, _types.arrayClass, "length", "()I" );
                    jmethodID optMethod = Mengine_JNI_GetMethodID( _jenv, _types.arrayClass, "opt", "(I)Ljava/lang/Object;" );
                    jint length = Mengine_JNI_CallIntMethod( _jenv, _jobject, lengthMethod );

                    if( Mengine_JNI_ExceptionCheck( _jenv ) == JNI_TRUE )
                    {
                        return false;
                    }

                    jpp::array array = jpp::make_array();

                    if( array.invalid() == true )
                    {
                        return false;
                    }

                    uint32_t childDepth = _depth + 1;

                    for( jint index = 0; index != length; ++index )
                    {
                        jobject element = Mengine_JNI_CallObjectMethod( _jenv, _jobject, optMethod, index );

                        if( Mengine_JNI_ExceptionCheck( _jenv ) == JNI_TRUE )
                        {
                            Mengine_JNI_DeleteLocalRef( _jenv, element );

                            return false;
                        }

                        jpp::object value;
                        bool successful = Detail::makeJSON( _jenv, element, _types, childDepth, &value );
                        Mengine_JNI_DeleteLocalRef( _jenv, element );

                        if( successful == false )
                        {
                            return false;
                        }

                        array.push_back( value );
                        jpp::array::size_type size = array.size();
                        jpp::array::size_type expectedSize = static_cast<jpp::array::size_type>(index) + 1;

                        if( size != expectedSize )
                        {
                            return false;
                        }
                    }

                    *_json = array;
                }
                else
                {
                    return false;
                }

                if( _json->invalid() == true )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJavaJSONObjectElement( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key )
        {
            jobject value = Detail::getJSONObjectValue( _jenv, _jclass, _jobject, _key, "opt", "(Ljava/lang/String;)Ljava/lang/Object;" );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJavaJSONObjectObject( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key )
        {
            jobject value = Detail::getJSONObjectValue( _jenv, _jclass, _jobject, _key, "optJSONObject", "(Ljava/lang/String;)Lorg/json/JSONObject;" );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJavaJSONObjectArray( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key )
        {
            jobject value = Detail::getJSONObjectValue( _jenv, _jclass, _jobject, _key, "optJSONArray", "(Ljava/lang/String;)Lorg/json/JSONArray;" );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidGetJavaJSONObjectBoolean( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, bool _default )
        {
            if( _jobject == nullptr )
            {
                return _default;
            }

            jmethodID method = Mengine_JNI_GetMethodID( _jenv, _jclass, "optBoolean", "(Ljava/lang/String;Z)Z" );
            jstring key = Mengine_JNI_NewStringUTF( _jenv, _key );
            jboolean defaultValue = _default == true ? JNI_TRUE : JNI_FALSE;
            jboolean value = Mengine_JNI_CallBooleanMethod( _jenv, _jobject, method, key, defaultValue );
            Mengine_JNI_DeleteLocalRef( _jenv, key );
            Helper::AndroidEnvExceptionCheck( _jenv );
            if( value != JNI_TRUE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidGetJavaJSONObjectInt( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jint _default )
        {
            if( _jobject == nullptr )
            {
                return _default;
            }

            jmethodID method = Mengine_JNI_GetMethodID( _jenv, _jclass, "optInt", "(Ljava/lang/String;I)I" );
            jstring key = Mengine_JNI_NewStringUTF( _jenv, _key );
            jint value = Mengine_JNI_CallIntMethod( _jenv, _jobject, method, key, _default );
            Mengine_JNI_DeleteLocalRef( _jenv, key );
            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidGetJavaJSONObjectLong( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jlong _default )
        {
            if( _jobject == nullptr )
            {
                return _default;
            }

            jmethodID method = Mengine_JNI_GetMethodID( _jenv, _jclass, "optLong", "(Ljava/lang/String;J)J" );
            jstring key = Mengine_JNI_NewStringUTF( _jenv, _key );
            jlong value = Mengine_JNI_CallLongMethod( _jenv, _jobject, method, key, _default );
            Mengine_JNI_DeleteLocalRef( _jenv, key );
            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jdouble AndroidGetJavaJSONObjectDouble( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jdouble _default )
        {
            if( _jobject == nullptr )
            {
                return _default;
            }

            jmethodID method = Mengine_JNI_GetMethodID( _jenv, _jclass, "optDouble", "(Ljava/lang/String;D)D" );
            jstring key = Mengine_JNI_NewStringUTF( _jenv, _key );
            jdouble value = Mengine_JNI_CallDoubleMethod( _jenv, _jobject, method, key, _default );
            Mengine_JNI_DeleteLocalRef( _jenv, key );
            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        String AndroidGetJavaJSONObjectString( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, const Char * _default )
        {
            String value( _default );
            jobject element = Helper::AndroidGetJavaJSONObjectElement( _jenv, _jclass, _jobject, _key );

            if( element == nullptr )
            {
                return value;
            }

            jclass stringClass = Mengine_JNI_GetClassString( _jenv );
            jboolean isString = Mengine_JNI_IsInstanceOf( _jenv, element, stringClass );
            Mengine_JNI_DeleteLocalRef( _jenv, stringClass );

            if( isString != JNI_TRUE )
            {
                Mengine_JNI_DeleteLocalRef( _jenv, element );

                return value;
            }

            jstring string = static_cast<jstring>(element);
            bool successful = Detail::makeString( _jenv, string, &value );
            Mengine_JNI_DeleteLocalRef( _jenv, element );
            Helper::AndroidEnvExceptionCheck( _jenv );

            if( successful == false )
            {
                String defaultValue( _default );

                return defaultValue;
            }

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidMakeJSONFromJObject( JNIEnv * _jenv, jobject _jobject, jpp::object * const _json )
        {
            jint localReferenceCapacity = 16;
            if( Mengine_JNI_PushLocalFrame( _jenv, localReferenceCapacity ) != JNI_OK )
            {
                Helper::AndroidEnvExceptionCheck( _jenv );

                return false;
            }

            Detail::AndroidJSONTypesDesc types;
            types.booleanClass = Mengine_JNI_GetClassBoolean( _jenv );
            types.integerClass = Mengine_JNI_GetClassInteger( _jenv );
            types.longClass = Mengine_JNI_GetClassLong( _jenv );
            types.byteClass = Mengine_JNI_FindClass( _jenv, "java/lang/Byte" );
            types.shortClass = Mengine_JNI_FindClass( _jenv, "java/lang/Short" );
            types.floatClass = Mengine_JNI_GetClassFloat( _jenv );
            types.doubleClass = Mengine_JNI_GetClassDouble( _jenv );
            types.stringClass = Mengine_JNI_GetClassString( _jenv );
            types.objectClass = Mengine_JNI_GetClassJSONObject( _jenv );
            types.arrayClass = Mengine_JNI_GetClassJSONArray( _jenv );
            jfieldID nullField = Mengine_JNI_GetStaticFieldID( _jenv, types.objectClass, "NULL", "Ljava/lang/Object;" );
            types.nullValue = Mengine_JNI_GetStaticObjectField( _jenv, types.objectClass, nullField );

            jpp::object json;
            bool successful = Detail::makeJSON( _jenv, _jobject, types, 0, &json );
            Mengine_JNI_PopLocalFrame( _jenv, nullptr );
            Helper::AndroidEnvExceptionCheck( _jenv );

            if( successful == false )
            {
                return false;
            }

            *_json = json;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
