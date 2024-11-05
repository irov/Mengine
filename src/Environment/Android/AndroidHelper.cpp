#include "AndroidHelper.h"

#include "Interface/AndroidKernelServiceInterface.h"

#include "AndroidEnv.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Error.h"
#include "Kernel/StringCopy.h"
#include "Kernel/StringSlice.h"
#include "Kernel/Logger.h"

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
            jclass ccls = _jenv->FindClass("java/lang/Class");

            jmethodID jmethodID_getName = _jenv->GetMethodID( ccls, "getName", "()Ljava/lang/String;" );

            MENGINE_ASSERTION_FATAL( jmethodID_getName != nullptr, "invalid get android method 'getName()Ljava/lang/String;'" );

            jstring obj_class_name = (jstring)_jenv->CallObjectMethod( _jclass, jmethodID_getName );
            Helper::AndroidEnvExceptionCheck( _jenv );

            const Char * obj_class_name_str = _jenv->GetStringUTFChars( obj_class_name, nullptr );
            jsize obj_class_name_size = _jenv->GetStringLength( obj_class_name );

            StaticString<1024> value( obj_class_name_str, obj_class_name_size );

            _jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
            _jenv->DeleteLocalRef( obj_class_name );

            _jenv->DeleteLocalRef( ccls );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... )
        {
            jclass jclass_name = Mengine::Mengine_JNI_FindClass( _jenv, _name );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jmethodID jmethodID_method = _jenv->GetStaticMethodID( jclass_name, _method, _signature );

            MENGINE_ASSERTION_FATAL( jmethodID_method != nullptr, "invalid get android method '%s::%s' with signature '%s'"
                , _name
                , _method
                , _signature
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallStaticVoidMethodV( jclass_name, jmethodID_method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_name );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... )
        {
            jclass jclass_name = Mengine::Mengine_JNI_FindClass( _jenv, _name );

            Helper::AndroidEnvExceptionCheck( _jenv );

            jmethodID jmethodID_method = _jenv->GetStaticMethodID( jclass_name, _method, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodID_method != nullptr, "invalid get android method '%s::%s' with signature '%s'"
                , _name
                , _method
                , _signature
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallStaticObjectMethodV( jclass_name, jmethodID_method, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_name );

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
            jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );

            jmethodID Boolean_constructor = _jenv->GetMethodID( jclass_Boolean, "<init>", "(Z)V" );

            MENGINE_ASSERTION( Boolean_constructor != nullptr, "invalid get android method 'Boolean <init> (Z)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Boolean, Boolean_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Boolean '%s'"
                , _value == true ? "true" : "false"
            );

            _jenv->DeleteLocalRef( jclass_Boolean );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectCharacter( JNIEnv * _jenv, Char _value )
        {
            jclass jclass_Character = _jenv->FindClass( "java/lang/Character" );

            jmethodID Character_constructor = _jenv->GetMethodID( jclass_Character, "<init>", "(C)V" );

            MENGINE_ASSERTION( jclass_Character != nullptr, "invalid get android method 'Character <init> (C)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Character, Character_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Character '%c' [%u]"
                , _value
                , (uint32_t)_value
            );

            _jenv->DeleteLocalRef( jclass_Character );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectInteger( JNIEnv * _jenv, int32_t _value )
        {
            jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );

            jmethodID Integer_constructor = _jenv->GetMethodID( jclass_Integer, "<init>", "(I)V" );

            MENGINE_ASSERTION( Integer_constructor != nullptr, "invalid get android method 'Integer <init> (I)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Integer, Integer_constructor, (jint)_value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Integer '%d'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Integer );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectLong( JNIEnv * _jenv, int64_t _value )
        {
            jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );

            jmethodID Long_constructor = _jenv->GetMethodID( jclass_Long, "<init>", "(J)V" );

            MENGINE_ASSERTION( Long_constructor != nullptr, "invalid get android method 'Long <init> (J)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Long, Long_constructor, (jlong)_value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Long '%" MENGINE_PRId64 "'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Long );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectFloat( JNIEnv * _jenv, float _value )
        {
            jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );

            jmethodID Float_constructor = _jenv->GetMethodID( jclass_Float, "<init>", "(F)V" );

            MENGINE_ASSERTION( Float_constructor != nullptr, "invalid get android method 'Float <init> (F)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Float, Float_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Float '%f'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Float );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectDouble( JNIEnv * _jenv, double _value )
        {
            jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );

            jmethodID Double_constructor = _jenv->GetMethodID( jclass_Double, "<init>", "(D)V" );

            MENGINE_ASSERTION( Double_constructor != nullptr, "invalid get android method 'Double <init> (D)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Double, Double_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Double '%lf'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Double );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const Char * _value )
        {
            jclass jclass_String = _jenv->FindClass( "java/lang/String" );

            jmethodID String_constructor = _jenv->GetMethodID( jclass_String, "<init>", "(Ljava/lang/String;)V" );

            MENGINE_ASSERTION( String_constructor != nullptr, "invalid get android method 'String <init> (Ljava/lang/String;)V'" );

            jstring value_jstring = _jenv->NewStringUTF( _value );

            jobject value_jobject = _jenv->NewObject( jclass_String, String_constructor, value_jstring );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create String '%s'"
                , _value
            );

            _jenv->DeleteLocalRef( value_jstring );
            _jenv->DeleteLocalRef( jclass_String );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const String & _value )
        {
            const Char * value_str = _value.c_str();

            jobject value_jobject = Helper::AndroidMakeJObjectString(_jenv, value_str);

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const ConstString & _value )
        {
            const Char * value_str = _value.c_str();

            jobject value_jobject = Helper::AndroidMakeJObjectString(_jenv, value_str);

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectArrayList( JNIEnv * _jenv, int32_t _count )
        {
            jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

            jmethodID ArrayList_constructor = _jenv->GetMethodID( jclass_ArrayList, "<init>", "(I)V" );

            MENGINE_ASSERTION( ArrayList_constructor != nullptr, "invalid get android method 'ArrayList <init> (I)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_ArrayList, ArrayList_constructor, (jint)_count );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create ArrayList '%d'"
                , _count
            );

            _jenv->DeleteLocalRef( jclass_ArrayList );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectHashMap( JNIEnv * _jenv, int32_t _count )
        {
            jclass jclass_HashMap = _jenv->FindClass( "java/util/HashMap" );

            jmethodID HashMap_constructor = _jenv->GetMethodID( jclass_HashMap, "<init>", "(I)V" );

            MENGINE_ASSERTION( HashMap_constructor != nullptr, "invalid get android method 'HashMap <init> (I)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_HashMap, HashMap_constructor, (jint)_count );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create HashMap '%d'"
                , _count
            );

            _jenv->DeleteLocalRef( jclass_HashMap );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        void * AndroidGetJObjectPointer( JNIEnv * _jenv, jobject _pointer )
        {
            jlong jvalue = Helper::AndroidGetJavaObjectValueLong( _jenv, _pointer );

            void * pointer = (void *)jvalue;

            return pointer;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidAddJObjectList( JNIEnv * _jenv, jobject _list, jobject _value )
        {
            jclass jclass_ArrayList = _jenv->FindClass( "java/util/List" );

            jmethodID ArrayList_add = _jenv->GetMethodID( jclass_ArrayList, "add", "(Ljava/lang/Object;)Z" );

            MENGINE_ASSERTION( ArrayList_add != nullptr, "invalid get android method 'java/util/List [add] (Ljava/lang/Object;)Z'" );

            jboolean result = _jenv->CallBooleanMethod( _list, ArrayList_add, _value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_ArrayList );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidPutJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key, jobject _value )
        {
            jclass jclass_Map = _jenv->FindClass( "java/util/Map" );

            jmethodID Map_put = _jenv->GetMethodID( jclass_Map, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" );

            MENGINE_ASSERTION( Map_put != nullptr, "invalid get android method 'java/util/Map [put] (Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;'" );

            jobject result = _jenv->CallObjectMethod( _map, Map_put, _key, _value );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Map );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key )
        {
            jclass jclass_Map = _jenv->FindClass( "java/util/Map" );

            jmethodID Map_get = _jenv->GetMethodID( jclass_Map, "get", "(Ljava/lang/Object;)Ljava/lang/Object;" );

            MENGINE_ASSERTION( Map_get != nullptr, "invalid get android method 'java/util/Map [get] (Ljava/lang/Object;)Ljava/lang/Object;'" );

            jobject value = _jenv->CallObjectMethod( _map, Map_get, _key );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Map );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidDeleteLocalRef( JNIEnv * _jenv, jobject _jobject )
        {
            _jenv->DeleteLocalRef( _jobject );
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidGetJavaObjectValueBoolean( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Boolean = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Boolean, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()Z'" );

            jboolean value = _jenv->CallBooleanMethod( _jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Boolean );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jchar AndroidGetJavaObjectValueCharacter( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Character = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Character, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

            jchar value = _jenv->CallCharMethod( _jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Character );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidGetJavaObjectValueInteger( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Integer = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Integer, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

            jint value = _jenv->CallIntMethod( _jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Integer );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidGetJavaObjectValueLong( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Long = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Long, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

            jlong value = _jenv->CallLongMethod(_jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Long );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jfloat AndroidGetJavaObjectValueFloat( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Float = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Float, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

            jfloat value = _jenv->CallFloatMethod( _jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Float );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        jdouble AndroidGetJavaObjectValueDouble( JNIEnv * _jenv, jobject _jobject )
        {
            jclass jclass_Double = _jenv->GetObjectClass( _jobject );

            jmethodID methodValue = _jenv->GetMethodID( jclass_Double, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

            jdouble value = _jenv->CallDoubleMethod( _jobject, methodValue );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_Double );

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
            const Char * value_str = _jenv->GetStringUTFChars( _value, nullptr );

            String value_string( value_str );

            _jenv->ReleaseStringUTFChars( _value, value_str );

            return value_string;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t AndroidCopyStringFromJString( JNIEnv * _jenv, jstring _value, Char * const _str, size_t _capacity )
        {
            const Char * value_str = _jenv->GetStringUTFChars( _value, nullptr );
            jsize value_size = _jenv->GetStringLength( _value );

            Helper::stringCopy( _str, value_str, _capacity );

            _jenv->ReleaseStringUTFChars( _value, value_str );

            return value_size;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaMap( JNIEnv * _jenv, jobject _jmap, const LambdaJavaMapForeach & _lambda )
        {
            jclass jclass_Map = _jenv->FindClass( "java/util/Map" );
            jclass jclass_Set = _jenv->FindClass( "java/util/Set" );
            jclass jclass_Iterator = _jenv->FindClass( "java/util/Iterator" );
            jclass jclass_MapEntry = _jenv->FindClass( "java/util/Map$Entry" );

            jmethodID jmethodID_Map_entrySet = _jenv->GetMethodID( jclass_Map, "entrySet", "()Ljava/util/Set;" );
            jmethodID jmethodID_Set_iterator = _jenv->GetMethodID( jclass_Set, "iterator", "()Ljava/util/Iterator;" );
            jmethodID jmethodID_Iterator_hasNext = _jenv->GetMethodID( jclass_Iterator, "hasNext", "()Z" );
            jmethodID jmethodID_Iterator_next = _jenv->GetMethodID( jclass_Iterator, "next", "()Ljava/lang/Object;" );
            jmethodID jmethodID_MapEntry_getKey = _jenv->GetMethodID( jclass_MapEntry, "getKey", "()Ljava/lang/Object;" );
            jmethodID jmethodID_MapEntry_getValue = _jenv->GetMethodID( jclass_MapEntry, "getValue", "()Ljava/lang/Object;" );

            jobject jset = _jenv->CallObjectMethod( _jmap, jmethodID_Map_entrySet );
            Helper::AndroidEnvExceptionCheck( _jenv );

            jobject jset_iterator = _jenv->CallObjectMethod( jset, jmethodID_Set_iterator );
            Helper::AndroidEnvExceptionCheck( _jenv );

            jboolean hasNext = _jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );
            Helper::AndroidEnvExceptionCheck( _jenv );

            while( hasNext == JNI_TRUE )
            {
                jobject jentry = _jenv->CallObjectMethod( jset_iterator, jmethodID_Iterator_next );
                Helper::AndroidEnvExceptionCheck( _jenv );

                jobject jkey = _jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getKey );
                Helper::AndroidEnvExceptionCheck( _jenv );

                jobject jvalue = (jstring)_jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getValue );
                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( jkey, jvalue );

                _jenv->DeleteLocalRef( jkey );
                _jenv->DeleteLocalRef( jvalue );
                _jenv->DeleteLocalRef( jentry );

                hasNext = _jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );
                Helper::AndroidEnvExceptionCheck( _jenv );
            }

            _jenv->DeleteLocalRef( jset_iterator );
            _jenv->DeleteLocalRef( jset );

            _jenv->DeleteLocalRef( jclass_Map );
            _jenv->DeleteLocalRef( jclass_Set );
            _jenv->DeleteLocalRef( jclass_Iterator );
            _jenv->DeleteLocalRef( jclass_MapEntry );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t AndroidGetJavaListSize( JNIEnv * _jenv, jobject _jlist )
        {
            jclass jclass_List = _jenv->FindClass( "java/util/List" );

            jmethodID List_size = _jenv->GetMethodID( jclass_List, "size", "()I");

            int list_size = _jenv->CallIntMethod( _jlist, List_size );
            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclass_List );

            return (uint32_t)list_size;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaList( JNIEnv * _jenv, jobject _jlist, const LambdaJavaListForeach & _lambda )
        {
            jclass jclass_List = _jenv->FindClass( "java/util/List" );

            jmethodID List_size = _jenv->GetMethodID( jclass_List, "size", "()I");

            MENGINE_ASSERTION_FATAL( List_size != nullptr, "invalid get android method 'java/lang/List [size] ()I'" );

            jmethodID List_get = _jenv->GetMethodID( jclass_List, "get", "(I)Ljava/lang/Object;");

            MENGINE_ASSERTION_FATAL( List_get != nullptr, "invalid get android method 'java/lang/List [get] (I)Ljava/lang/Object;'" );

            jsize list_size = _jenv->CallIntMethod( _jlist, List_size );
            Helper::AndroidEnvExceptionCheck( _jenv );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject list_obj = _jenv->CallObjectMethod( _jlist, List_get, index );
                Helper::AndroidEnvExceptionCheck( _jenv );

                _lambda( index, list_obj );

                _jenv->DeleteLocalRef( list_obj );
            }

            _jenv->DeleteLocalRef( jclass_List );
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidGetJavaRect( JNIEnv * _jenv, jobject _jrect, Viewport * const _viewport )
        {
            jclass jclass_Rect = _jenv->FindClass( "android/graphics/Rect" );

            jfieldID jfieldID_Rect_left = _jenv->GetFieldID( jclass_Rect, "left", "I" );
            jfieldID jfieldID_Rect_top = _jenv->GetFieldID( jclass_Rect, "top", "I" );
            jfieldID jfieldID_Rect_right = _jenv->GetFieldID( jclass_Rect, "right", "I" );
            jfieldID jfieldID_Rect_bottom = _jenv->GetFieldID( jclass_Rect, "bottom", "I" );

            jint left = _jenv->GetIntField( _jrect, jfieldID_Rect_left );
            jint top = _jenv->GetIntField( _jrect, jfieldID_Rect_top );
            jint right = _jenv->GetIntField( _jrect, jfieldID_Rect_right );
            jint bottom = _jenv->GetIntField( _jrect, jfieldID_Rect_bottom );

            _viewport->begin.x = (float)left;
            _viewport->begin.y = (float)top;
            _viewport->end.x = (float)right;
            _viewport->end.y = (float)bottom;

            _jenv->DeleteLocalRef( jclass_Rect );
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidWriteMemory( JNIEnv * _jenv, const Mengine::MemoryInterfacePtr & _memory, jobject _writer )
        {
            jclass jclass_Writer = _jenv->GetObjectClass( _writer );
            jmethodID jmethodID_Writer_write_String = _jenv->GetMethodID( jclass_Writer, "write", "(Ljava/lang/String;)V" );

            const Mengine::Char * data_value = _memory->getBuffer();
            size_t data_size = _memory->getSize();

            Mengine::Utf8 jvalue_str[1024 + 1] = {'\0'};
            bool result = Mengine::Helper::utf8Slice( data_value, data_size, jvalue_str, 1024, [_jenv, _writer, jmethodID_Writer_write_String]( const Mengine::Utf8 * _utf8 )
            {
                jstring jvalue = _jenv->NewStringUTF( _utf8 );

                if( jvalue == nullptr )
                {
                    LOGGER_ERROR( "invalid write memory string UTF8: %s"
                        , _utf8
                    );

                    return false;
                }

                _jenv->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
                _jenv->DeleteLocalRef( jvalue );

                return true;
            } );

            _jenv->DeleteLocalRef( jclass_Writer );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidGetActivityFilesDirCanonicalPath( JNIEnv * _jenv, Char * const _path )
        {
            jclass jclassMengineActivity = Mengine_JNI_GetJClassMengineActivity();
            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            jmethodID midContext_GetFilesDir = _jenv->GetMethodID( jclassMengineActivity, "getFilesDir", "()Ljava/io/File;" );

            jobject filesDir = _jenv->CallObjectMethod( jobjectMengineActivity, midContext_GetFilesDir );

            if( filesDir == nullptr )
            {
                return false;
            }

            jclass jclassFilesDir = _jenv->GetObjectClass( filesDir );

            jmethodID midFilesDir_getCanonicalPath = _jenv->GetMethodID( jclassFilesDir, "getCanonicalPath", "()Ljava/lang/String;" );

            _jenv->DeleteLocalRef( jclassFilesDir );

            jstring canonicalPath = (jstring)_jenv->CallObjectMethod( filesDir, midFilesDir_getCanonicalPath );

            _jenv->DeleteLocalRef( filesDir );

            const Char * canonicalPath_str = _jenv->GetStringUTFChars( canonicalPath, NULL );
            StdString::strcpy( _path, canonicalPath_str );
            _jenv->ReleaseStringUTFChars( canonicalPath, canonicalPath_str );

            _jenv->DeleteLocalRef( canonicalPath );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetActivitySurface( JNIEnv * _jenv )
        {
            jclass jclassMengineActivity = Mengine_JNI_GetJClassMengineActivity();
            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            jmethodID midMengineActivity_getNativeSurface = _jenv->GetMethodID( jclassMengineActivity, "getSurface", "()Landroid/view/Surface;" );

            jobject jSurface = _jenv->CallObjectMethod( jobjectMengineActivity, midMengineActivity_getNativeSurface );
            Helper::AndroidEnvExceptionCheck( _jenv );

            return jSurface;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidEnvExceptionCheck( JNIEnv * _jenv )
        {
            if( _jenv->ExceptionCheck() == JNI_FALSE )
            {
                return;
            }

            jthrowable jThrowable = _jenv->ExceptionOccurred();

            _jenv->ExceptionClear();

            jclass jclassMengineApplication = Mengine_JNI_GetJClassMengineApplication();

            if( jclassMengineApplication == nullptr )
            {
                _jenv->DeleteLocalRef( jThrowable );

                return;
            }

            jmethodID jmethodID_MengineApplication_onMengineCaughtException = _jenv->GetMethodID( jclassMengineApplication, "onMengineCaughtException", "(Ljava/lang/Throwable;)V" );

            if( jmethodID_MengineApplication_onMengineCaughtException == nullptr )
            {
                _jenv->DeleteLocalRef( jThrowable );

                return;
            }

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            if( jobjectMengineApplication == nullptr )
            {
                _jenv->DeleteLocalRef( jThrowable );

                return;
            }

            _jenv->CallVoidMethod( jobjectMengineApplication, jmethodID_MengineApplication_onMengineCaughtException, jThrowable );

            _jenv->DeleteLocalRef( jThrowable );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}