#pragma once

#include "Interface/MemoryInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"
#include "Kernel/ArrayString.h"
#include "Kernel/StaticString.h"
#include "Kernel/Viewport.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StaticString<1024> AndroidGetJavaClassName( JNIEnv * _jenv, jclass _jclass );
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... );
        jobject AndroidCallObjectStaticClassMethod( JNIEnv * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidMakeJObjectPointer( JNIEnv * _jenv, void * _value );
        jobject AndroidMakeJObjectBoolean( JNIEnv * _jenv, bool _value );
        jobject AndroidMakeJObjectCharacter( JNIEnv * _jenv, Char _value );
        jobject AndroidMakeJObjectInteger( JNIEnv * _jenv, int32_t _value );
        jobject AndroidMakeJObjectLong( JNIEnv * _jenv, int64_t _value );
        jobject AndroidMakeJObjectFloat( JNIEnv * _jenv, float _value );
        jobject AndroidMakeJObjectDouble( JNIEnv * _jenv, double _value );
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const Char * _value );
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const String & _value );
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const ConstString & _value );
        template<size_t N>
        jobject AndroidMakeJObjectString( JNIEnv * _jenv, const StaticString<N> & _value )
        {
            return Helper::AndroidMakeJObjectString( _jenv, _value.c_str() );
        }
        jobject AndroidMakeJObjectArrayList( JNIEnv * _jenv, int32_t _count );
        jobject AndroidMakeJObjectHashMap( JNIEnv * _jenv, int32_t _count );
        //////////////////////////////////////////////////////////////////////////
        void * AndroidGetJObjectPointer( JNIEnv * _jenv, jobject _pointer );
        jboolean AndroidAddJObjectList( JNIEnv * _jenv, jobject _list, jobject _value );
        jobject AndroidPutJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key, jobject _value );
        jobject AndroidGetJObjectMap( JNIEnv * _jenv, jobject _map, jobject _key );
        //////////////////////////////////////////////////////////////////////////
        void AndroidDeleteLocalRef( JNIEnv * _jenv, jobject _jobject );
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidGetJavaObjectValueBoolean( JNIEnv * _jenv, jobject _jobject );
        jchar AndroidGetJavaObjectValueCharacter( JNIEnv * _jenv, jobject _jobject );
        jint AndroidGetJavaObjectValueInteger( JNIEnv * _jenv, jobject _jobject );
        jlong AndroidGetJavaObjectValueLong( JNIEnv * _jenv, jobject _jobject );
        jfloat AndroidGetJavaObjectValueFloat( JNIEnv * _jenv, jobject _jobject );
        jdouble AndroidGetJavaObjectValueDouble( JNIEnv * _jenv, jobject _jobject );
        //////////////////////////////////////////////////////////////////////////
        ConstString AndroidMakeConstStringFromJString( JNIEnv * _jenv, jstring _value );
        FilePath AndroidMakeFilePathFromJString( JNIEnv * _jenv, jstring _value );
        String AndroidMakeStringFromJString( JNIEnv * _jenv, jstring _value );
        size_t AndroidCopyStringFromJString( JNIEnv * _jenv, jstring _value, Char * const _str, size_t _capacity );
        ///////////////////////////////////////////////////////////////////////
        template<size_t N>
        size_t AndroidCopyStringFromJString( JNIEnv * _jenv, jstring _value, StaticString<N> * const _str )
        {
            Char * data = _str->data();
            size_t capacity = _str->capacity();

            size_t size = Helper::AndroidCopyStringFromJString( _jenv, _value, data, capacity );

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jobject key, jobject value)> LambdaJavaMapForeach;
        void AndroidForeachJavaMap( JNIEnv * _jenv, jobject _jmap, const LambdaJavaMapForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jsize _index, jobject value)> LambdaJavaListForeach;
        uint32_t AndroidGetJavaListSize( JNIEnv * _jenv, jobject _jlist );
        void AndroidForeachJavaList( JNIEnv * _jenv, jobject _jlist, const LambdaJavaListForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        void AndroidGetJavaRect( JNIEnv * _jenv, jobject _jrect, Viewport * const _viewport );
        //////////////////////////////////////////////////////////////////////////
        bool AndroidWriteMemory( JNIEnv * _jenv, const MemoryInterfacePtr & _memory, jobject _writer );
        //////////////////////////////////////////////////////////////////////////
        bool AndroidGetApplicationFilesDirCanonicalPath( JNIEnv * _jenv, Char * const _path );
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetActivitySurface( JNIEnv * _jenv );
        //////////////////////////////////////////////////////////////////////////
        void AndroidEnvExceptionCheck( JNIEnv * _jenv );
        //////////////////////////////////////////////////////////////////////////
    }
}
