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

            jclass jclass_Boolean = Mengine_JNI_GetClassBoolean( _jenv );
            jclass jclass_Character = Mengine_JNI_GetClassCharacter( _jenv );
            jclass jclass_Integer = Mengine_JNI_GetClassInteger( _jenv );
            jclass jclass_Long = Mengine_JNI_GetClassLong( _jenv );
            jclass jclass_Float = Mengine_JNI_GetClassFloat( _jenv );
            jclass jclass_Double = Mengine_JNI_GetClassDouble( _jenv );
            jclass jclass_String = Mengine_JNI_GetClassString( _jenv );
            jclass jclass_Exception = Mengine_JNI_GetClassException( _jenv );
            jclass jclass_List = Mengine_JNI_GetClassList( _jenv );
            jclass jclass_Map = Mengine_JNI_GetClassMap( _jenv );
            jclass jclass_Set = Mengine_JNI_GetClassSet( _jenv );
            jclass jclass_Rect = Mengine_JNI_GetClassRect( _jenv );
            jclass jclass_JSONObject = Mengine_JNI_GetClassJSONObject( _jenv );
            jclass jclass_JSONArray = Mengine_JNI_GetClassJSONArray( _jenv );
            jclass jclass_MengineCallback = Mengine_JNI_GetClassMengineCallback( _jenv );

            if( _obj == nullptr )
            {
                py_value = _kernel->ret_none();
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Boolean ) == JNI_TRUE )
            {
                jboolean value = Helper::AndroidGetJavaObjectValueBoolean( _jenv, jclass_Boolean, _obj );

                py_value = _kernel->ret_bool( value );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Character ) == JNI_TRUE )
            {
                jchar value = Helper::AndroidGetJavaObjectValueCharacter( _jenv, jclass_Character, _obj );

                Char value_str[2] = {(Char)value, '\0'};
                py_value = _kernel->string_from_char_size( value_str, 1 );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Integer ) == JNI_TRUE )
            {
                jint value = Helper::AndroidGetJavaObjectValueInteger( _jenv, jclass_Integer, _obj );

                py_value = _kernel->ptr_int32( value );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Long ) == JNI_TRUE )
            {
                jlong value = Helper::AndroidGetJavaObjectValueLong( _jenv, jclass_Long, _obj );

                py_value = _kernel->ptr_int64( value );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Float ) == JNI_TRUE )
            {
                jfloat value = Helper::AndroidGetJavaObjectValueFloat( _jenv, jclass_Float, _obj );

                py_value = _kernel->ptr_float( value );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Double ) == JNI_TRUE )
            {
                jdouble value = Helper::AndroidGetJavaObjectValueDouble( _jenv, jclass_Double, _obj );

                py_value = _kernel->ptr_double( value );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_String ) == JNI_TRUE )
            {
                jstring jobj_string = (jstring)_obj;

                const Char * obj_str = Mengine_JNI_GetStringUTFChars( _jenv, jobj_string, nullptr );
                jsize obj_size = Mengine_JNI_GetStringLength( _jenv, jobj_string );

                py_value = _kernel->string_from_char_size( obj_str, obj_size );

                Mengine_JNI_ReleaseStringUTFChars( _jenv, jobj_string, obj_str );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Exception ) == JNI_TRUE )
            {
                jmethodID jmethodID_getMessage = Mengine_JNI_GetMethodID( _jenv, jclass_Exception, "getMessage", "()Ljava/lang/String;" );

                jstring jstring_message = (jstring)Mengine_JNI_CallObjectMethod( _jenv, _obj, jmethodID_getMessage );

                const Char * obj_str = Mengine_JNI_GetStringUTFChars( _jenv, jstring_message, nullptr );

                py_value = _kernel->exception_new( obj_str );

                Mengine_JNI_ReleaseStringUTFChars( _jenv, jstring_message, obj_str );
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_List ) == JNI_TRUE )
            {
                uint32_t list_size = Helper::AndroidGetJavaListSize( _jenv, jclass_List, _obj );

                PyObject * py_list = _kernel->list_new( list_size );

                Helper::AndroidForeachJavaList( _jenv, jclass_List, _obj, [_kernel, _jenv, py_list, _doc](jsize _index, jobject _jvalue) {
                    PyObject * py_obj = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->list_setitem( py_list, _index, py_obj );

                    _kernel->decref( py_obj );
                });

                py_value = py_list;
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Map ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                Helper::AndroidForeachJavaMap( _jenv, jclass_Map, _obj, [_kernel, _jenv, _doc, py_dict](jobject _jkey, jobject _jvalue) {
                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );
                });

                py_value = py_dict;
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Set ) == JNI_TRUE )
            {
                PyObject * py_set = _kernel->set_new();

                Helper::AndroidForeachJavaSet( _jenv, jclass_Set, _obj, [_kernel, _jenv, _doc, py_set](jobject _jvalue) {
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->set_set( py_set, py_value );

                    _kernel->decref( py_value );
                });

                py_value = py_set;
            }
            else if( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_Rect ) == JNI_TRUE )
            {
                Viewport viewport;
                Helper::AndroidGetJavaRect( _jenv, jclass_Rect, _obj, &viewport );

                py_value = pybind::ptr( _kernel, viewport );
            }
            else if ( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_JSONObject ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                Helper::AndroidForeachJavaJSONObject( _jenv, jclass_JSONObject, _obj, [_kernel, _jenv, _doc, py_dict](jobject _jkey, jobject _jvalue) {
                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );
                });

                py_value = py_dict;
            }
            else if ( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_JSONArray ) == JNI_TRUE )
            {
                PyObject * py_list = _kernel->list_new( 0 );

                Helper::AndroidForeachJavaJSONArray( _jenv, jclass_JSONArray, _obj, [_kernel, _jenv, _doc, py_list](jint _index, jobject _jvalue) {
                    MENGINE_UNUSED( _index );

                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->list_appenditem( py_list, py_value );

                    _kernel->decref( py_value );
                });

                py_value = py_list;
            }
            else if ( Mengine_JNI_IsInstanceOf( _jenv, _obj, jclass_MengineCallback ) == JNI_TRUE )
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
                jclass jclass_Class = Mengine_JNI_GetObjectClass( _jenv, _obj );

                LOGGER_ERROR( "unsupported java argument type '%s'"
                    , Helper::AndroidGetJavaClassName( _jenv, jclass_Class ).c_str()
                );

                Mengine_JNI_DeleteLocalRef( _jenv, jclass_Class );
            }

            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Boolean );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Character );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Integer );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Long );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Float );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Double );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_String );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Exception );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_List );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Map );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Set );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_Rect );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_JSONObject );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_JSONArray );
            Mengine_JNI_DeleteLocalRef( _jenv, jclass_MengineCallback );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonListMakeJavaObject( JNIEnv * _jenv, const pybind::list & _list )
        {
            pybind::list::size_type s = _list.size();

            jclass jclass_ArrayList = Mengine_JNI_GetClassArrayList( _jenv );

            jmethodID jmethod_List_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_ArrayList, "<init>", "(I)V" );

            jobject jobject_list = Mengine_JNI_NewObject( _jenv, jclass_ArrayList, jmethod_List_constructor, (jsize)s );

            for( const pybind::object & o : _list )
            {
                jobject jobject_element = Helper::androidNativePythonMakeJavaObject( _jenv, o );

                Helper::AndroidAddJObjectList( _jenv, jobject_list, jobject_element );

                Mengine_JNI_DeleteLocalRef( _jenv, jobject_element );
            }

            return jobject_list;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonDictMakeJavaObject( JNIEnv * _jenv, const pybind::dict & _dict )
        {
            pybind::dict::size_type s = _dict.size();

            jclass jclass_HashMap = Mengine_JNI_GetClassHashMap( _jenv );

            jmethodID jmethod_HashMap_constructor = Mengine_JNI_GetMethodID( _jenv, jclass_HashMap, "<init>", "()V" );

            jobject jobject_map = Mengine_JNI_NewObject( _jenv, jclass_HashMap, jmethod_HashMap_constructor );

            for( const pybind::dict_pair_value & pair : _dict )
            {
                const Char * key = pair.key();
                pybind::object value = pair.value();

                jstring jstring_key = Mengine_JNI_NewStringUTF( _jenv, key );
                jobject jobject_element = Helper::androidNativePythonMakeJavaObject( _jenv, value );

                Helper::AndroidPutJObjectMap( _jenv, jobject_map, jstring_key, jobject_element );

                Mengine_JNI_DeleteLocalRef( _jenv, jstring_key );
                Mengine_JNI_DeleteLocalRef( _jenv, jobject_element );
            }

            return jobject_map;
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