#pragma once

#include "Interface/MemoryInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"
#include "Kernel/ArrayString.h"
#include "Kernel/StaticString.h"
#include "Kernel/Viewport.h"
#include "Kernel/Params.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StaticString<1024> AndroidGetJavaClassName( MengineJNIEnvThread * _jenv, jclass _jclass );
        jclass AndroidEnvFindClass( MengineJNIEnvThread * _jenv, const Char * _className );
        jobject AndroidEnvGetObjectFragment( MengineJNIEnvThread * _jenv, const Char * _fragment );
        jmethodID AndroidEnvGetMethodFragment( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature );
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidStaticClassMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectStaticClassMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _method, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
        MENGINE_NODISCARD jobject AndroidMakeJObjectPointer( MengineJNIEnvThread * _jenv, void * _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectBoolean( MengineJNIEnvThread * _jenv, bool _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectCharacter( MengineJNIEnvThread * _jenv, Char _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectInteger( MengineJNIEnvThread * _jenv, int32_t _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectLong( MengineJNIEnvThread * _jenv, int64_t _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectFloat( MengineJNIEnvThread * _jenv, float _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectDouble( MengineJNIEnvThread * _jenv, double _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectString( MengineJNIEnvThread * _jenv, const Char * _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectString( MengineJNIEnvThread * _jenv, const String & _value );
        MENGINE_NODISCARD jobject AndroidMakeJObjectString( MengineJNIEnvThread * _jenv, const ConstString & _value );
        template<size_t N>
        MENGINE_NODISCARD jobject AndroidMakeJObjectString( MengineJNIEnvThread * _jenv, const StaticString<N> & _value )
        {
            return Helper::AndroidMakeJObjectString( _jenv, _value.c_str() );
        }
        MENGINE_NODISCARD jobject AndroidMakeJObjectArrayList( MengineJNIEnvThread * _jenv, int32_t _count );
        MENGINE_NODISCARD jobject AndroidMakeJObjectHashMap( MengineJNIEnvThread * _jenv, int32_t _count );
        MENGINE_NODISCARD jobject AndroidMakeJObjectHashMap( MengineJNIEnvThread * _jenv, int32_t _count );
        MENGINE_NODISCARD jobject AndroidMakeJObjectHashMap( MengineJNIEnvThread * _jenv, const Params & _params );
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidGetJObjectEnum( MengineJNIEnvThread * _jenv, const Char * _className, const Char * _enumName );
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidAddJObjectList( MengineJNIEnvThread * _jenv, jobject _list, jobject _value );
        void AndroidPutJObjectMap( MengineJNIEnvThread * _jenv, jobject _map, jobject _key, jobject _value );
        MENGINE_NODISCARD jobject AndroidGetJObjectMap( MengineJNIEnvThread * _jenv, jobject _map, jobject _key );
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidGetJavaObjectValueBoolean( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        jchar AndroidGetJavaObjectValueCharacter( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        jint AndroidGetJavaObjectValueInteger( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        jlong AndroidGetJavaObjectValueLong( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        jfloat AndroidGetJavaObjectValueFloat( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        jdouble AndroidGetJavaObjectValueDouble( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jobject );
        //////////////////////////////////////////////////////////////////////////
        ConstString AndroidMakeConstStringFromJString( MengineJNIEnvThread * _jenv, jstring _value );
        FilePath AndroidMakeFilePathFromJString( MengineJNIEnvThread * _jenv, jstring _value );
        String AndroidMakeStringFromJString( MengineJNIEnvThread * _jenv, jstring _value );
        size_t AndroidCopyStringFromJString( MengineJNIEnvThread * _jenv, jstring _value, Char * const _str, size_t _capacity );
        ///////////////////////////////////////////////////////////////////////
        template<size_t N>
        size_t AndroidCopyStringFromJString( MengineJNIEnvThread * _jenv, jstring _value, StaticString<N> * const _str )
        {
            Char * data = _str->data();
            size_t capacity = _str->capacity();

            size_t size = Helper::AndroidCopyStringFromJString( _jenv, _value, data, capacity );

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jobject key, jobject value )> LambdaJavaJSONObjectForeach;
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaJSONObject( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jjson, const LambdaJavaJSONObjectForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jint index, jobject value )> LambdaJavaJSONArrayForeach;
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaJSONArray( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jarray, const LambdaJavaJSONArrayForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jobject value )> LambdaJavaSetForeach;
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaSet( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jset, const LambdaJavaSetForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jobject key, jobject value )> LambdaJavaMapForeach;
        //////////////////////////////////////////////////////////////////////////
        void AndroidForeachJavaMap( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jmap, const LambdaJavaMapForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jsize _index, jobject value )> LambdaJavaListForeach;
        //////////////////////////////////////////////////////////////////////////
        uint32_t AndroidGetJavaListSize( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jlist );
        void AndroidForeachJavaList( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jlist, const LambdaJavaListForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        void AndroidGetJavaRect( MengineJNIEnvThread * _jenv, jclass _jclass, jobject _jrect, Viewport * const _viewport );
        //////////////////////////////////////////////////////////////////////////
        bool AndroidGetApplicationFilesDirCanonicalPath( MengineJNIEnvThread * _jenv, Char * const _path );
        //////////////////////////////////////////////////////////////////////////
        MENGINE_NODISCARD jobject AndroidGetActivitySurface( MengineJNIEnvThread * _jenv );
        //////////////////////////////////////////////////////////////////////////
        void AndroidEnvExceptionCheck( MengineJNIEnvThread * _jenv );
        //////////////////////////////////////////////////////////////////////////
    }
}