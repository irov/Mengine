#include "AndroidNativePythonHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Python/PythonIncluder.h"

#include "AndroidNativePythonCallback.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        PyObject * androidNativePythonMakePyObject( pybind::kernel_interface * _kernel, JNIEnv * _jenv, jobject _obj, const DocumentInterfacePtr & _doc )
        {
            PyObject * py_value = nullptr;

            jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );
            jclass jclass_Character = _jenv->FindClass( "java/lang/Character" );
            jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );
            jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );
            jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );
            jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );
            jclass jclass_String = _jenv->FindClass( "java/lang/String" );
            jclass jclass_Exception = _jenv->FindClass( "java/lang/Exception" );
            jclass jclass_List = _jenv->FindClass( "java/util/List" );
            jclass jclass_Map = _jenv->FindClass( "java/util/Map" );
            jclass jclass_Set = _jenv->FindClass( "java/util/Set" );
            jclass jclass_Rect = _jenv->FindClass( "android/graphics/Rect" );
            jclass jclass_JSONObject = _jenv->FindClass( "org/json/JSONObject" );
            jclass jclass_JSONArray = _jenv->FindClass( "org/json/JSONArray" );
            jclass jclass_MengineCallback = Mengine_JNI_FindClass( _jenv, "org/Mengine/Base/MengineCallback" );

            if( _obj == nullptr )
            {
                py_value = _kernel->ret_none();
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Boolean ) == JNI_TRUE )
            {
                jboolean value = Helper::AndroidGetJavaObjectValueBoolean( _jenv, _obj );

                py_value = _kernel->ret_bool( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Character ) == JNI_TRUE )
            {
                jchar value = Helper::AndroidGetJavaObjectValueCharacter( _jenv, _obj );

                Char value_str[2] = {(Char)value, '\0'};
                py_value = _kernel->string_from_char_size( value_str, 1 );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Integer ) == JNI_TRUE )
            {
                jint value = Helper::AndroidGetJavaObjectValueInteger( _jenv, _obj );

                py_value = _kernel->ptr_int32( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Long ) == JNI_TRUE )
            {
                jlong value = Helper::AndroidGetJavaObjectValueLong( _jenv, _obj );

                py_value = _kernel->ptr_int64( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Float ) == JNI_TRUE )
            {
                jfloat value = Helper::AndroidGetJavaObjectValueFloat( _jenv, _obj );

                py_value = _kernel->ptr_float( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Double ) == JNI_TRUE )
            {
                jdouble value = Helper::AndroidGetJavaObjectValueDouble( _jenv, _obj );

                py_value = _kernel->ptr_double( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_String ) == JNI_TRUE )
            {
                jstring jobj_string = (jstring)_obj;

                const Char * obj_str = _jenv->GetStringUTFChars( jobj_string, nullptr );
                jsize obj_size = _jenv->GetStringLength( jobj_string );

                py_value = _kernel->string_from_char_size( obj_str, obj_size );

                _jenv->ReleaseStringUTFChars( jobj_string, obj_str );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Exception ) == JNI_TRUE )
            {
                jmethodID jmethodID_getMessage = _jenv->GetMethodID( jclass_Exception, "getMessage", "()Ljava/lang/String;" );

                jstring jstring_message = (jstring)_jenv->CallObjectMethod( _obj, jmethodID_getMessage );

                const Char * obj_str = _jenv->GetStringUTFChars( jstring_message, nullptr );

                py_value = _kernel->exception_new( obj_str );

                _jenv->ReleaseStringUTFChars( jstring_message, obj_str );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_List ) == JNI_TRUE )
            {
                uint32_t list_size = Helper::AndroidGetJavaListSize( _jenv, _obj );

                PyObject * py_list = _kernel->list_new( list_size );

                Helper::AndroidForeachJavaList( _jenv, _obj, [_kernel, _jenv, py_list, _doc](jsize _index, jobject _jvalue) {
                    PyObject * py_obj = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->list_setitem( py_list, _index, py_obj );

                    _kernel->decref( py_obj );
                });

                py_value = py_list;
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Map ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                Helper::AndroidForeachJavaMap( _jenv, _obj, [_kernel, _jenv, _doc, py_dict](jobject _jkey, jobject _jvalue) {
                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );
                });

                py_value = py_dict;
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Rect ) == JNI_TRUE )
            {
                Viewport viewport;
                Helper::AndroidGetJavaRect( _jenv, _obj, &viewport );

                py_value = pybind::ptr( _kernel, viewport );
            }
            else if ( _jenv->IsInstanceOf( _obj, jclass_JSONObject ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                Helper::AndroidForeachJavaJSONObject( _jenv, _obj, [_kernel, _jenv, _doc, py_dict](jobject _jkey, jobject _jvalue) {
                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );
                });

                py_value = py_dict;
            }
            else if ( _jenv->IsInstanceOf( _obj, jclass_JSONArray ) == JNI_TRUE )
            {
                PyObject * py_list = _kernel->list_new( 0 );

                Helper::AndroidForeachJavaJSONArray( _jenv, _obj, [_kernel, _jenv, _doc, py_list](jint _index, jobject _jvalue) {
                    MENGINE_UNUSED( _index );

                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->list_appenditem( py_list, py_value );

                    _kernel->decref( py_value );
                });

                py_value = py_list;
            }
            else if ( _jenv->IsInstanceOf( _obj, jclass_MengineCallback ) == JNI_TRUE )
            {
                AndroidNativePythonCallbackPtr functor = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "AndroidNativePython" ), STRINGIZE_STRING_LOCAL( "AndroidNativePythonCallback" ), _doc );

                functor->setKernel( _kernel );
                functor->setJavaFunctor( _jenv, _obj );

                PyObject * py_functor = pybind::ptr( _kernel, functor );

                py_value = py_functor;
            }
            else
            {
                jclass cls_obj = _jenv->GetObjectClass( _obj );

                LOGGER_ERROR( "unsupported java argument type '%s'"
                    , Helper::AndroidGetJavaClassName( _jenv, cls_obj ).c_str()
                );

                _jenv->DeleteLocalRef( cls_obj );
            }

            _jenv->DeleteLocalRef( jclass_Boolean );
            _jenv->DeleteLocalRef( jclass_Character );
            _jenv->DeleteLocalRef( jclass_Integer );
            _jenv->DeleteLocalRef( jclass_Long );
            _jenv->DeleteLocalRef( jclass_Float );
            _jenv->DeleteLocalRef( jclass_Double );
            _jenv->DeleteLocalRef( jclass_String );
            _jenv->DeleteLocalRef( jclass_Exception );
            _jenv->DeleteLocalRef( jclass_List );
            _jenv->DeleteLocalRef( jclass_Map );
            _jenv->DeleteLocalRef( jclass_Set );
            _jenv->DeleteLocalRef( jclass_Rect );
            _jenv->DeleteLocalRef( jclass_MengineCallback );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonListMakeJavaObject( JNIEnv * _jenv, const pybind::list & _list )
        {
            pybind::list::size_type s = _list.size();

            jclass jclass_ArrayList = _jenv->FindClass( "java/util/ArrayList" );

            jmethodID jmethodID_List_constructor = _jenv->GetMethodID( jclass_ArrayList, "<init>", "(I)V" );

            jobject jlist = _jenv->NewObject( jclass_ArrayList, jmethodID_List_constructor, (jsize)s );

            for( const pybind::object & o : _list )
            {
                jobject jelement = Helper::androidNativePythonMakeJavaObject( _jenv, o );

                Helper::AndroidAddJObjectList( _jenv, jlist, jelement );

                _jenv->DeleteLocalRef( jelement );
            }

            _jenv->DeleteLocalRef( jclass_ArrayList );

            return jlist;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonDictMakeJavaObject( JNIEnv * _jenv, const pybind::dict & _dict )
        {
            pybind::dict::size_type s = _dict.size();

            jclass jclass_HashMap = _jenv->FindClass( "java/util/HashMap" );

            jmethodID jmethodID_HashMap_constructor = _jenv->GetMethodID( jclass_HashMap, "<init>", "()V" );

            jobject jmap = _jenv->NewObject( jclass_HashMap, jmethodID_HashMap_constructor );

            for( const pybind::dict_pair_value & pair : _dict )
            {
                const Char * key = pair.key();
                pybind::object value = pair.value();

                jstring jkey = _jenv->NewStringUTF( key );
                jobject jelement = Helper::androidNativePythonMakeJavaObject( _jenv, value );

                Helper::AndroidPutJObjectMap( _jenv, jmap, jkey, jelement );

                _jenv->DeleteLocalRef( jkey );
                _jenv->DeleteLocalRef( jelement );
            }

            _jenv->DeleteLocalRef( jclass_HashMap );

            return jmap;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonMakeJavaObject( JNIEnv * _jenv, const pybind::object & _obj )
        {
            jobject jresult;

            if( _obj.is_none() == true )
            {
                jresult = nullptr;
            }
            else if( _obj.is_bool() == true )
            {
                bool value = _obj.extract();

                jresult = Helper::AndroidMakeJObjectBoolean( _jenv, value );
            }
            else if( _obj.is_integer() == true )
            {
                int32_t value = _obj.extract();

                jresult = Helper::AndroidMakeJObjectInteger( _jenv, value );
            }
            else if( _obj.is_long() == true )
            {
                int64_t value = _obj.extract();

                jresult = Helper::AndroidMakeJObjectLong( _jenv, value );
            }
            else if( _obj.is_float() == true )
            {
                double value = _obj.extract();

                jresult = Helper::AndroidMakeJObjectDouble( _jenv, value );
            }
            else if( _obj.is_string() == true )
            {
                const Char * value = _obj.extract();

                jresult = Helper::AndroidMakeJObjectString( _jenv, value );
            }
            else if( _obj.is_list() == true )
            {
                pybind::list l = _obj.extract();

                jobject jlist = Helper::androidNativePythonListMakeJavaObject( _jenv, l );

                jresult = jlist;
            }
            else if( _obj.is_dict() == true )
            {
                pybind::dict d = _obj.extract();

                jobject jmap = Helper::androidNativePythonDictMakeJavaObject( _jenv, d );

                jresult = jmap;
            }
            else
            {
                LOGGER_ERROR( "unsupported python argument '%s'"
                    , _obj.repr_type().c_str()
                );

                return nullptr;
            }

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}