#include "AndroidHelper.h"

#include "Environment/Android/AndroidKernelServiceInterface.h"

#include "AndroidEnv.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionUtf8.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Error.h"
#include "Kernel/StringSlice.h"
#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdString.h"
#include "Config/Utf8.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StaticString<1024> AndroidGetJavaClassName( JNIEnv * _jenv, jclass _jclass )
        {
            jclass jclass_Class = Mengine_JNI_GetClassClass( _jenv );

            jmethodID jmethodID_getName = Mengine_JNI_GetMethodID( _jenv, jclass_Class, "getName", "()Ljava/lang/String;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Class );

            jstring jobject_ClassName = (jstring)Mengine_JNI_CallObjectMethod( _jenv, _jclass, jmethodID_getName );

            Helper::AndroidEnvExceptionCheck( _jenv );

            const Char * obj_class_name_str = Mengine_JNI_GetStringUTFChars( _jenv, jobject_ClassName, nullptr );
            jsize obj_class_name_size = Mengine_JNI_GetStringLength( _jenv, jobject_ClassName );

            StaticString<1024> value( obj_class_name_str, obj_class_name_size );

            Mengine_JNI_ReleaseStringUTFChars( _jenv, jobject_ClassName, obj_class_name_str );
            Mengine_JNI_DeleteLocalRef( _jenv, jobject_ClassName );
			
            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jclass AndroidEnvFindClass( JNIEnv * _jenv, const Char * _className )
        {
            jclass jclass_FindClass = Mengine_JNI_LoadClass( _jenv, _className );

            if( jclass_FindClass == nullptr )
            {
                Helper::AndroidEnvExceptionCheck( _jenv );

                MENGINE_ERROR_FATAL( "invalid find android class: %s"
                    , _className
                );

                return nullptr;
            }

            return jclass_FindClass;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidEnvGetObjectFragment( JNIEnv * _jenv, const Char * _fragment )
        {
            jobject jobject_Fragment = Mengine_JNI_GetObjectFragment( _jenv, _fragment );

            if( jobject_Fragment == nullptr )
            {
                Helper::AndroidEnvExceptionCheck( _jenv );

                MENGINE_ERROR_FATAL( "invalid get object fragment: %s"
                    , _fragment
                );

                return nullptr;
            }

            return jobject_Fragment;
        }
        //////////////////////////////////////////////////////////////////////////
        jmethodID AndroidEnvGetMethodFragment( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature )
        {
            jmethodID jmethod_Fragment = Mengine_JNI_GetMethodFragment( _jenv, _fragment, _method, _signature );

            if( jmethod_Fragment == nullptr )
            {
                Helper::AndroidEnvExceptionCheck( _jenv );

                MENGINE_ERROR_FATAL( "invalid fragment: %s get method: %s with signature: %s"
                    , _fragment
                    , _method
                    , _signature
                );

                return nullptr;
            }

            return jmethod_Fragment;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... )
        {
            jclass jclass_name = Helper::AndroidEnvFindClass( _jenv, _name );

            jmethodID jmethodID_method = Mengine_JNI_GetStaticMethodID( _jenv, jclass_name, _method, _signature );

            MENGINE_ASSERTION_FATAL( jmethodID_method != nullptr, "invalid get android method: %s::%s with signature: %s"
                , _name
                , _method
                , _signature
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallStaticVoidMethodV( _jenv, jclass_name, jmethodID_method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_name );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... )
        {
            jclass jclass_name = Helper::AndroidEnvFindClass( _jenv, _name );

            jmethodID jmethodID_method = Mengine_JNI_GetStaticMethodID( _jenv, jclass_name, _method, _signature );

            MENGINE_ASSERTION_FATAL( jmethodID_method != nullptr, "invalid get android method: %s::%s with signature: %s"
                , _name
                , _method
                , _signature
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = Mengine_JNI_CallStaticObjectMethodV( _jenv, jclass_name, jmethodID_method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_name );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectPointer( JNIEnv * _jenv, void * _value )
        {
            int64_t pointer_value = (int64_t)_value;

            jobject jpointer = Helper::AndroidMakeJObjectLong( _jenv, pointer_value );

            return jpointer;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectBoolean( JNIEnv * _jenv, bool _value )
        {
            jclass jclass_Boolean = Mengine_JNI_GetClassBoolean( _jenv );

            jmethodID Boolean_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Boolean, "<init>", "(Z)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Boolean, Boolean_constructor, _value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Boolean );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectCharacter( JNIEnv * _jenv, Char _value )
        {
            jclass jclass_Character = Mengine_JNI_GetClassCharacter( _jenv );

            jmethodID Character_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Character, "<init>", "(C)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Character, Character_constructor, _value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Character );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectInteger( JNIEnv * _jenv, int32_t _value )
        {
            jclass jclass_Integer = Mengine_JNI_GetClassInteger( _jenv );

            jmethodID Integer_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Integer, "<init>", "(I)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Integer, Integer_constructor, (jint)_value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Integer );
			
            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectLong( JNIEnv * _jenv, int64_t _value )
        {
            jclass jclass_Long = Mengine_JNI_GetClassLong( _jenv );

            jmethodID Long_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Long, "<init>", "(J)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Long, Long_constructor, (jlong)_value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Long );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectFloat( JNIEnv * _jenv, float _value )
        {
            jclass jclass_Float = Mengine_JNI_GetClassFloat( _jenv );

            jmethodID Float_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Float, "<init>", "(F)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Float, Float_constructor, _value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Float );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectDouble( JNIEnv * _jenv, double _value )
        {
            jclass jclass_Double = Mengine_JNI_GetClassDouble( _jenv );

            jmethodID Double_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_Double, "<init>", "(D)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_Double, Double_constructor, _value );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Double );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jstring AndroidMakeJObjectString( JNIEnv * _jenv, const Char * _value )
        {
            MENGINE_ASSERTION_VALIDATE_UTF8( _value, MENGINE_UNKNOWN_SIZE );

            jstring value_jstring = Mengine_JNI_NewStringUTF( _jenv, _value );

            return value_jstring;
        }
        //////////////////////////////////////////////////////////////////////////
        jstring AndroidMakeJObjectString( JNIEnv * _jenv, const String & _value )
        {
            const Char * value_str = _value.c_str();

            jstring value_jobject = Helper::AndroidMakeJObjectString( _jenv, value_str );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jstring AndroidMakeJObjectString( JNIEnv * _jenv, const ConstString & _value )
        {
            const Char * value_str = _value.c_str();

            jstring value_jobject = Helper::AndroidMakeJObjectString( _jenv, value_str );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectArrayList( JNIEnv * _jenv, int32_t _count )
        {
            jclass jclass_ArrayList = Mengine_JNI_GetClassArrayList( _jenv );

            jmethodID ArrayList_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_ArrayList, "<init>", "(I)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_ArrayList, ArrayList_constructor, (jint)_count );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_ArrayList );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectHashMap( JNIEnv * _jenv, int32_t _count )
        {
            jclass jclass_HashMap = Mengine_JNI_GetClassHashMap( _jenv );

            jmethodID HashMap_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_HashMap, "<init>", "(I)V" );

            jobject value_jobject = Mengine_JNI_NewObject( _jenv, jclass_HashMap, HashMap_constructor, (jint)_count );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_HashMap );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_NODISCARD jobject AndroidMakeJObjectHashMap( JNIEnv * _jenv, const Params & _params )
        {
            Params::size_type size = _params.size();

            jobject jmap = Helper::AndroidMakeJObjectHashMap( _jenv, size );

            for( auto && [key, value] : _params ) {
                jobject jkey = Helper::AndroidMakeJObjectString( _jenv, key );

                Helper::visit(value, [_jenv, jmap, jkey]( const ParamNull & _element ) {
                    MENGINE_UNUSED( _element );

                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, nullptr );
                }, [_jenv, jmap, jkey]( const ParamBool & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectBoolean( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, [_jenv, jmap, jkey]( const ParamInteger & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectInteger( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, [_jenv, jmap, jkey]( const ParamDouble & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectDouble( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, [_jenv, jmap, jkey]( const ParamString & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectString( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, []( const ParamWString & _element ) {
                    MENGINE_UNUSED(_element);

                    MENGINE_ASSERTION_FATAL( false, "not support ParamWString" );
                }, [_jenv, jmap, jkey]( const ParamConstString & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectString( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, [_jenv, jmap, jkey]( const ParamFilePath & _element ) {
                    jobject jvalue = Helper::AndroidMakeJObjectString( _jenv, _element );
                    Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jvalue );
                    Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                }, []( const ParamFactorablePtr & _element ) {
                    MENGINE_UNUSED(_element);

                    MENGINE_ASSERTION_FATAL( false, "not support ParamFactorablePtr" );
                });

                Mengine_JNI_DeleteLocalRef( _jenv, jkey );
            }

            return jmap;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJObjectEnum( JNIEnv * _jenv, const Char * _className, const Char * _enumName )
        {
            jclass jclass_enum = Helper::AndroidEnvFindClass( _jenv, _className );

            Char signature[256] = { '\0' };
            StdString::strcpy_safe( signature, "L", 256 );
            StdString::strcat_safe( signature, _className, 256 );
            StdString::strcat_safe( signature, ";", 256 );

            jfieldID jfield_enum = Mengine_JNI_GetStaticFieldID( _jenv, jclass_enum, _enumName, signature );

            jobject jobject_enum = Mengine_JNI_GetStaticObjectField( _jenv, jclass_enum, jfield_enum );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_enum );

            return jobject_enum;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidAddJObjectList( JNIEnv * _jenv, jobject _list, jobject _value )
        {
            jclass jclass_ArrayList = Mengine_JNI_GetClassList( _jenv );

            jmethodID ArrayList_add = Mengine_JNI_GetMethodID( _jenv, jclass_ArrayList, "add", "(Ljava/lang/Object;)Z" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_ArrayList );

            jboolean result = Mengine_JNI_CallBooleanMethod( _jenv, _list, ArrayList_add, _value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidPutJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key, jobject _value )
        {
            jclass jclass_Map = Mengine_JNI_GetClassMap( _jenv );

            jmethodID Map_put = Mengine_JNI_GetMethodID( _jenv, jclass_Map, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Map );

            jobject jresult = Mengine_JNI_CallObjectMethod( _jenv, _map, Map_put, _key, _value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jresult );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key )
        {
            jclass jclass_Map = Mengine_JNI_GetClassMap( _jenv );

            jmethodID Map_get = Mengine_JNI_GetMethodID( _jenv, jclass_Map, "get", "(Ljava/lang/Object;)Ljava/lang/Object;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Map );

            jobject value = Mengine_JNI_CallObjectMethod( _jenv, _map, Map_get, _key );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidGetJavaObjectValueBoolean( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()Z'" );

            jboolean value = Mengine_JNI_CallBooleanMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jchar AndroidGetJavaObjectValueCharacter( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

            jchar value = Mengine_JNI_CallCharMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidGetJavaObjectValueInteger( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

            jint value = Mengine_JNI_CallIntMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidGetJavaObjectValueLong( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

            jlong value = Mengine_JNI_CallLongMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jfloat AndroidGetJavaObjectValueFloat( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

            jfloat value = Mengine_JNI_CallFloatMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jdouble AndroidGetJavaObjectValueDouble( JNIEnv * _jenv, jclass _jclass, jobject _jobject )
        {
            jmethodID jmethod_Value = Mengine_JNI_GetMethodID( _jenv, _jclass, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( jmethod_Value != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

            jdouble value = Mengine_JNI_CallDoubleMethod( _jenv, _jobject, jmethod_Value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString AndroidMakeConstStringFromJString( JNIEnv * _jenv, jstring _value )
        {
            ConstString value_cstr;
            ANDROID_KERNEL_SERVICE()
                ->stringize( _jenv, _value, &value_cstr );

            return value_cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath AndroidMakeFilePathFromJString( JNIEnv * _jenv, jstring _value )
        {
            ConstString value_cstr;
            ANDROID_KERNEL_SERVICE()
                ->stringize( _jenv, _value, &value_cstr );

            return FilePath( value_cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        String AndroidMakeStringFromJString( JNIEnv * _jenv, jstring _value )
        {
            const Char * value_str = Mengine_JNI_GetStringUTFChars( _jenv, _value, nullptr );
            jsize value_size = Mengine_JNI_GetStringLength( _jenv, _value );

            String value_string( value_str, value_size );

            Mengine_JNI_ReleaseStringUTFChars( _jenv, _value, value_str );

            return value_string;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t AndroidCopyStringFromJString( JNIEnv * _jenv, jstring _value, Char * const _str, size_t _capacity )
        {
            const Char * value_str = Mengine_JNI_GetStringUTFChars( _jenv, _value, nullptr );
            jsize value_size = Mengine_JNI_GetStringLength( _jenv, _value );

            StdString::strcpy_safe( _str, value_str, _capacity );

            Mengine_JNI_ReleaseStringUTFChars( _jenv, _value, value_str );

            return value_size;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaJSONObject( JNIEnv * _jenv, jclass _jclass, jobject _jjson, const LambdaJavaJSONObjectForeach & _lambda )
        {
            jmethodID JSONObject_keys = Mengine_JNI_GetMethodID( _jenv, _jclass, "keys", "()Ljava/util/Iterator;" );

            jclass jclass_Iterator = Mengine_JNI_GetClassIterator( _jenv );

            jmethodID Iterator_hasNext = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "hasNext", "()Z" );
            jmethodID Iterator_next = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "next", "()Ljava/lang/Object;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Iterator );

            jmethodID JSONObject_opt = Mengine_JNI_GetMethodID( _jenv, _jclass, "opt", "(Ljava/lang/String;)Ljava/lang/Object;" );

            jobject jkeys = Mengine_JNI_CallObjectMethod( _jenv, _jjson, JSONObject_keys );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jboolean hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jkeys, Iterator_hasNext );

            Helper::AndroidEnvExceptionCheck( _jenv );

            while( hasNext == JNI_TRUE )
            {
                jobject jkey = Mengine_JNI_CallObjectMethod( _jenv, jkeys, Iterator_next );

                Helper::AndroidEnvExceptionCheck( _jenv );

                jobject jvalue = Mengine_JNI_CallObjectMethod( _jenv, _jjson, JSONObject_opt, jkey );

                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( jkey, jvalue );

                Mengine_JNI_DeleteLocalRef( _jenv, jkey );
                Mengine_JNI_DeleteLocalRef( _jenv, jvalue );

                hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jkeys, Iterator_hasNext );

                Helper::AndroidEnvExceptionCheck( _jenv );
            }

            Mengine_JNI_DeleteLocalRef( _jenv, jkeys );
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaJSONArray( JNIEnv * _jenv, jclass _jclass, jobject _jarray, const LambdaJavaJSONArrayForeach & _lambda )
        {
            jmethodID JSONArray_length = Mengine_JNI_GetMethodID( _jenv, _jclass, "length", "()I" );
            jmethodID JSONArray_get = Mengine_JNI_GetMethodID( _jenv, _jclass, "get", "(I)Ljava/lang/Object;" );

            jint jarray_length = Mengine_JNI_CallIntMethod( _jenv, _jarray, JSONArray_length );

            Helper::AndroidEnvExceptionCheck( _jenv );

            for( jint index = 0; index != jarray_length; ++index )
            {
                jobject jvalue = Mengine_JNI_CallObjectMethod( _jenv, _jarray, JSONArray_get, index );

                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( index, jvalue );

                Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJavaJSONArrayElement( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index )
        {
            jmethodID JSONArray_get = Mengine_JNI_GetMethodID( _jenv, _jclass, "get", "(I)Ljava/lang/Object;" );

            jobject jvalue = Mengine_JNI_CallObjectMethod( _jenv, _jarray, JSONArray_get, _index );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidGetJavaJSONArrayLength( JNIEnv * _jenv, jclass _jclass, jobject _jarray )
        {
            jmethodID JSONArray_length = Mengine_JNI_GetMethodID( _jenv, _jclass, "length", "()I" );

            jint jarray_length = Mengine_JNI_CallIntMethod( _jenv, _jarray, JSONArray_length );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jarray_length;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidGetJavaJSONArrayBoolean( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jboolean _default )
        {
            jmethodID JSONArray_optBoolean = Mengine_JNI_GetMethodID( _jenv, _jclass, "optBoolean", "(IZ)Z" );

            jboolean jvalue = Mengine_JNI_CallBooleanMethod( _jenv, _jarray, JSONArray_optBoolean, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidGetJavaJSONArrayInt( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jint _default )
        {
            jmethodID JSONArray_optInt = Mengine_JNI_GetMethodID( _jenv, _jclass, "optInt", "(II)I" );

            jint jvalue = Mengine_JNI_CallIntMethod( _jenv, _jarray, JSONArray_optInt, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidGetJavaJSONArrayLong( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jlong _default )
        {
            jmethodID JSONArray_optLong = Mengine_JNI_GetMethodID( _jenv, _jclass, "optLong", "(IJ)J" );

            jlong jvalue = Mengine_JNI_CallLongMethod(_jenv, _jarray, JSONArray_optLong, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jfloat AndroidGetJavaJSONArrayFloat( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jfloat _default )
        {
            jmethodID JSONArray_optFloat = Mengine_JNI_GetMethodID( _jenv, _jclass, "optFloat", "(IF)F" );

            jfloat jvalue = Mengine_JNI_CallFloatMethod( _jenv, _jarray, JSONArray_optFloat, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jdouble AndroidGetJavaJSONArrayDouble( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jdouble _default )
        {
            jmethodID JSONArray_optDouble = Mengine_JNI_GetMethodID( _jenv, _jclass, "optDouble", "(ID)D" );

            jdouble jvalue = Mengine_JNI_CallDoubleMethod( _jenv, _jarray, JSONArray_optDouble, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        jstring AndroidGetJavaJSONArrayString( JNIEnv * _jenv, jclass _jclass, jobject _jarray, jint _index, jstring _default )
        {
            jmethodID JSONArray_optString = Mengine_JNI_GetMethodID( _jenv, _jclass, "optString", "(ILjava/lang/String;)Ljava/lang/String;" );

            jstring jvalue = (jstring)Mengine_JNI_CallObjectMethod( _jenv, _jarray, JSONArray_optString, _index, _default );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jvalue;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaSet( JNIEnv * _jenv, jclass _jclass, jobject _jset, const LambdaJavaSetForeach & _lambda )
        {
            jclass jclass_Iterator = Mengine_JNI_GetClassIterator( _jenv );

            jmethodID jmethodID_Set_iterator = Mengine_JNI_GetMethodID( _jenv, _jclass, "iterator", "()Ljava/util/Iterator;" );
            jmethodID jmethodID_Iterator_hasNext = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "hasNext", "()Z" );
            jmethodID jmethodID_Iterator_next = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "next", "()Ljava/lang/Object;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Iterator );

            jobject jobject_iterator = Mengine_JNI_CallObjectMethod( _jenv, _jset, jmethodID_Set_iterator );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jboolean hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jobject_iterator, jmethodID_Iterator_hasNext );

            Helper::AndroidEnvExceptionCheck( _jenv );

            while( hasNext == JNI_TRUE )
            {
                jobject jelement = Mengine_JNI_CallObjectMethod( _jenv, jobject_iterator, jmethodID_Iterator_next );

                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( jelement );

                Mengine_JNI_DeleteLocalRef( _jenv, jelement );

                hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jobject_iterator, jmethodID_Iterator_hasNext );

                Helper::AndroidEnvExceptionCheck( _jenv );
            }

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_iterator );
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaMap( JNIEnv * _jenv, jclass _jclass, jobject _jmap, const LambdaJavaMapForeach & _lambda )
        {
            jclass jclass_Set = Mengine_JNI_GetClassSet( _jenv );
            jclass jclass_Iterator = Mengine_JNI_GetClassIterator( _jenv );
            jclass jclass_MapEntry = Mengine_JNI_GetClassMapEntry( _jenv );

            jmethodID jmethodID_Map_entrySet = Mengine_JNI_GetMethodID( _jenv, _jclass, "entrySet", "()Ljava/util/Set;" );
            jmethodID jmethodID_Set_iterator = Mengine_JNI_GetMethodID( _jenv, jclass_Set, "iterator", "()Ljava/util/Iterator;" );
            jmethodID jmethodID_Iterator_hasNext = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "hasNext", "()Z" );
            jmethodID jmethodID_Iterator_next = Mengine_JNI_GetMethodID( _jenv, jclass_Iterator, "next", "()Ljava/lang/Object;" );
            jmethodID jmethodID_MapEntry_getKey = Mengine_JNI_GetMethodID( _jenv, jclass_MapEntry, "getKey", "()Ljava/lang/Object;" );
            jmethodID jmethodID_MapEntry_getValue = Mengine_JNI_GetMethodID( _jenv, jclass_MapEntry, "getValue", "()Ljava/lang/Object;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Set );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Iterator );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_MapEntry );

            jobject jset = Mengine_JNI_CallObjectMethod( _jenv, _jmap, jmethodID_Map_entrySet );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jobject jset_iterator = Mengine_JNI_CallObjectMethod( _jenv, jset, jmethodID_Set_iterator );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jboolean hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jset_iterator, jmethodID_Iterator_hasNext );

            Helper::AndroidEnvExceptionCheck( _jenv );

            while( hasNext == JNI_TRUE )
            {
                jobject jentry = Mengine_JNI_CallObjectMethod( _jenv, jset_iterator, jmethodID_Iterator_next );

                Helper::AndroidEnvExceptionCheck( _jenv );

                jobject jkey = Mengine_JNI_CallObjectMethod( _jenv, jentry, jmethodID_MapEntry_getKey );

                Helper::AndroidEnvExceptionCheck( _jenv );

                jobject jvalue = (jstring)Mengine_JNI_CallObjectMethod( _jenv, jentry, jmethodID_MapEntry_getValue );

                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( jkey, jvalue );

                Mengine_JNI_DeleteLocalRef( _jenv, jkey );
                Mengine_JNI_DeleteLocalRef( _jenv, jvalue );
                Mengine_JNI_DeleteLocalRef( _jenv, jentry );

                hasNext = Mengine_JNI_CallBooleanMethod( _jenv, jset_iterator, jmethodID_Iterator_hasNext );

                Helper::AndroidEnvExceptionCheck( _jenv );
            }

            Mengine_JNI_DeleteLocalRef( _jenv, jset_iterator );
            Mengine_JNI_DeleteLocalRef( _jenv, jset );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t AndroidGetJavaListSize( JNIEnv * _jenv, jclass _jclass, jobject _jlist )
        {
            jmethodID List_size = Mengine_JNI_GetMethodID( _jenv, _jclass, "size", "()I");

            int list_size = Mengine_JNI_CallIntMethod( _jenv, _jlist, List_size );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return (uint32_t)list_size;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaList( JNIEnv * _jenv, jclass _jclass, jobject _jlist, const LambdaJavaListForeach & _lambda )
        {
            jmethodID List_size = Mengine_JNI_GetMethodID( _jenv, _jclass, "size", "()I");
            jmethodID List_get = Mengine_JNI_GetMethodID( _jenv, _jclass, "get", "(I)Ljava/lang/Object;");

            jsize list_size = Mengine_JNI_CallIntMethod( _jenv, _jlist, List_size );

            Helper::AndroidEnvExceptionCheck( _jenv );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject jobject_element = Mengine_JNI_CallObjectMethod( _jenv, _jlist, List_get, index );

                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( index, jobject_element );

                Mengine_JNI_DeleteLocalRef( _jenv, jobject_element );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidGetJavaRect( JNIEnv * _jenv, jclass _jclass, jobject _jrect, Viewport * const _viewport )
        {
            jfieldID jfieldID_Rect_left = Mengine_JNI_GetFieldID( _jenv, _jclass, "left", "I" );
            jfieldID jfieldID_Rect_top = Mengine_JNI_GetFieldID( _jenv, _jclass, "top", "I" );
            jfieldID jfieldID_Rect_right = Mengine_JNI_GetFieldID( _jenv, _jclass, "right", "I" );
            jfieldID jfieldID_Rect_bottom = Mengine_JNI_GetFieldID( _jenv, _jclass, "bottom", "I" );

            jint left = Mengine_JNI_GetIntField( _jenv, _jrect, jfieldID_Rect_left );
            jint top = Mengine_JNI_GetIntField( _jenv, _jrect, jfieldID_Rect_top );
            jint right = Mengine_JNI_GetIntField( _jenv, _jrect, jfieldID_Rect_right );
            jint bottom = Mengine_JNI_GetIntField( _jenv, _jrect, jfieldID_Rect_bottom );

            _viewport->begin.x = (float)left;
            _viewport->begin.y = (float)top;
            _viewport->end.x = (float)right;
            _viewport->end.y = (float)bottom;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidGetApplicationFilesDirCanonicalPath( JNIEnv * _jenv, Char * const _path )
        {
            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            jmethodID midMengineApplication_getFilesDir = Mengine_JNI_GetMethodApplication( _jenv, "getFilesDir", "()Ljava/io/File;" );

            jobject filesDir = Mengine_JNI_CallObjectMethod( _jenv, jobject_MengineApplication, midMengineApplication_getFilesDir );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );

            if( filesDir == nullptr )
            {
                return false;
            }

            jclass jclass_FilesDir = Mengine_JNI_GetObjectClass( _jenv, filesDir );

            jmethodID midFilesDir_getCanonicalPath = Mengine_JNI_GetMethodID( _jenv, jclass_FilesDir, "getCanonicalPath", "()Ljava/lang/String;" );

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_FilesDir );

            jstring jstring_CanonicalPath = (jstring)Mengine_JNI_CallObjectMethod( _jenv, filesDir, midFilesDir_getCanonicalPath );

            Mengine_JNI_DeleteLocalRef( _jenv, filesDir );

            if( jstring_CanonicalPath == nullptr )
            {
                return false;
            }

            const Char * canonicalPath_str = Mengine_JNI_GetStringUTFChars( _jenv, jstring_CanonicalPath, NULL );
            StdString::strcpy_safe( _path, canonicalPath_str, MENGINE_MAX_PATH );
            Mengine_JNI_ReleaseStringUTFChars( _jenv, jstring_CanonicalPath, canonicalPath_str );

            Mengine_JNI_DeleteLocalRef( _jenv, jstring_CanonicalPath );

            Helper::pathCorrectFolderPathA( _path, MENGINE_PATH_FORWARDSLASH );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetActivitySurface( JNIEnv * _jenv )
        {
            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            if( jobject_MengineActivity == nullptr )
            {
                return nullptr;
            }

            jmethodID midMengineActivity_getNativeSurface = Mengine_JNI_GetMethodActivity( _jenv, "getSurface", "()Landroid/view/Surface;" );

            jobject jSurface = Mengine_JNI_CallObjectMethod( _jenv, jobject_MengineActivity, midMengineActivity_getNativeSurface );
            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );

            return jSurface;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidEnvExceptionCheck( JNIEnv * _jenv )
        {
            if( Mengine_JNI_ExceptionCheck( _jenv ) == JNI_FALSE )
            {
                return;
            }

            jthrowable jThrowable = Mengine_JNI_ExceptionOccurred( _jenv );

            Mengine_JNI_ExceptionClear( _jenv );

            jmethodID jmethodMengineFragmentEngine_caughtException = Mengine_JNI_GetMethodFragment( _jenv, "MengineFragmentEngine", "caughtException", "(Ljava/lang/Throwable;)V" );

            if( jmethodMengineFragmentEngine_caughtException == nullptr )
            {
                Mengine_JNI_DeleteLocalRef( _jenv, jThrowable );

                return;
            }

            jobject jobject_MengineFragmentEngine = Mengine::Mengine_JNI_GetObjectFragment( _jenv, "MengineFragmentEngine" );

            if( jobject_MengineFragmentEngine == nullptr )
            {
                Mengine_JNI_DeleteLocalRef( _jenv, jThrowable );

                return;
            }

            Mengine_JNI_CallVoidMethod( _jenv, jobject_MengineFragmentEngine, jmethodMengineFragmentEngine_caughtException, jThrowable );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineFragmentEngine );
            Mengine_JNI_DeleteLocalRef( _jenv, jThrowable );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}