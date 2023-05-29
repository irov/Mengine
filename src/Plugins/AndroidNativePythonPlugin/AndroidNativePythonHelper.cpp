#include "AndroidNativePythonHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Python/PythonIncluder.h"

#include "AndroidNativePythonFunctorVoid.h"
#include "AndroidNativePythonFunctorBoolean.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        PyObject * androidNativePythonMakePyObject( pybind::kernel_interface * _kernel, JNIEnv * _jenv, jobject _obj, const DocumentPtr & _doc )
        {
            PyObject * py_value = nullptr;

            jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );
            jclass jclass_Character = _jenv->FindClass( "java/lang/Character" );
            jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );
            jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );
            jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );
            jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );
            jclass jclass_String = _jenv->FindClass( "java/lang/String" );
            jclass jclass_List = _jenv->FindClass( "java/util/List" );
            jclass jclass_Map = _jenv->FindClass( "java/util/Map" );
            jclass jclass_Set = _jenv->FindClass( "java/util/Set" );
            jclass jclass_MengineFunctorVoid = _jenv->FindClass( "org/Mengine/Base/MengineFunctorVoid" );
            jclass jclass_MengineFunctorBoolean = _jenv->FindClass( "org/Mengine/Base/MengineFunctorBoolean" );

            if( _obj == nullptr )
            {
                py_value = _kernel->ret_none();
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_Boolean ) == JNI_TRUE )
            {
                jboolean value = Helper::getJavaObjectValueBoolean( _jenv, _obj );

                py_value = _kernel->ret_bool( value );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_Character ) == JNI_TRUE )
            {
                jchar value = Helper::getJavaObjectValueCharacter( _jenv, _obj );

                Char value_str[2] = {(Char)value, '\0'};
                py_value = _kernel->string_from_char_size( value_str, 1 );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_Integer ) == JNI_TRUE )
            {
                jint value = Helper::getJavaObjectValueInteger( _jenv, _obj );;

                py_value = _kernel->ptr_int32( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Long ) == JNI_TRUE )
            {
                jlong value = Helper::getJavaObjectValueLong( _jenv, _obj );

                py_value = _kernel->ptr_int64( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Float ) == JNI_TRUE )
            {
                jfloat value = Helper::getJavaObjectValueFloat( _jenv, _obj );

                py_value = _kernel->ptr_float( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Double ) == JNI_TRUE )
            {
                jdouble value = Helper::getJavaObjectValueDouble( _jenv, _obj );

                py_value = _kernel->ptr_double( value );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_String ) == JNI_TRUE )
            {
                const Char * obj_str = _jenv->GetStringUTFChars( (jstring)_obj, nullptr );

                py_value = _kernel->string_from_char( obj_str );

                _jenv->ReleaseStringUTFChars( (jstring)_obj, obj_str );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_List ) == JNI_TRUE )
            {
                uint32_t list_size = Helper::getJavaListSize( _jenv, _obj );

                PyObject * py_list = _kernel->list_new( list_size );

                Helper::foreachJavaList( _jenv, _obj, [_kernel, _jenv, py_list, _doc]( jsize _index, jobject _jvalue )
                {
                    PyObject * py_obj = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                    _kernel->list_setitem( py_list, _index, py_obj );

                    _kernel->decref( py_obj );
                });

                py_value = py_list;
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Map ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                Helper::foreachJavaMap(_jenv, _obj, [_kernel, _jenv, _doc, py_dict](jobject _jkey, jobject _jvalue) {
                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, _jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );
                });

                py_value = py_dict;
            }
            else if ( _jenv->IsInstanceOf( _obj, jclass_MengineFunctorVoid ) == JNI_TRUE )
            {
                AndroidNativePythonFunctorVoidPtr functor = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "AndroidNativePython" ), STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorVoid" ), _doc );

                functor->setKernel( _kernel );
                functor->setJavaFunctor( _obj );

                PyObject * py_functor = pybind::ptr( _kernel, functor );

                py_value = py_functor;
            }
            else if ( _jenv->IsInstanceOf( _obj, jclass_MengineFunctorBoolean ) == JNI_TRUE )
            {
                AndroidNativePythonFunctorBooleanPtr functor = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "AndroidNativePython" ), STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorBoolean" ), _doc );

                functor->setKernel( _kernel );
                functor->setJavaFunctor( _obj );

                PyObject * py_functor = pybind::ptr( _kernel, functor );

                py_value = py_functor;
            }
            else
            {
                jclass cls_obj = _jenv->GetObjectClass( _obj );

                LOGGER_ERROR( "unsupported java argument type '%s'"
                    , Helper::getJavaClassName( _jenv, cls_obj ).c_str()
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
            _jenv->DeleteLocalRef( jclass_List );
            _jenv->DeleteLocalRef( jclass_Map );
            _jenv->DeleteLocalRef( jclass_Set );
            _jenv->DeleteLocalRef( jclass_MengineFunctorVoid );
            _jenv->DeleteLocalRef( jclass_MengineFunctorBoolean );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject androidNativePythonMakeJavaObject( JNIEnv * _jenv, pybind::kernel_interface * _kernel, PyObject * _obj )
        {
            jobject jresult;

            if( _kernel->is_none( _obj ) == true )
            {
                jclass jclass_Object = _jenv->FindClass( "java/lang/Object" );

                jmethodID constructor = _jenv->GetMethodID( jclass_Object, "<init>", "()V" );

                MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] ()V'" );

                jresult = _jenv->NewObject( jclass_Object, constructor );

                _jenv->DeleteLocalRef( jclass_Object );
            }
            else if( _kernel->bool_check( _obj ) == true )
            {
                bool value = pybind::extract<bool>( _kernel, _obj );

                jresult = Helper::makeJObjectBoolean( _jenv, value );
            }
            else if( _kernel->int_check( _obj ) == true )
            {
                int32_t value = pybind::extract<int32_t>( _kernel, _obj );

                jresult = Helper::makeJObjectInteger( _jenv, value );
            }
            else if( _kernel->long_check( _obj ) == true )
            {
                int64_t value = pybind::extract<int64_t>( _kernel, _obj );

                jresult = Helper::makeJObjectLong( _jenv, value );
            }
            else if( _kernel->float_check( _obj ) == true )
            {
                double value = pybind::extract<double>( _kernel, _obj );

                jresult = Helper::makeJObjectDouble( _jenv, value );
            }
            else if( _kernel->string_check( _obj ) == true )
            {
                const Char * value = _kernel->string_to_char( _obj );

                jresult = Helper::makeJObjectString( _jenv, value );
            }
            else
            {
                LOGGER_ERROR( "unsupported python argument '%s'"
                    , _kernel->object_repr_type( _obj ).c_str()
                );

                return nullptr;
            }

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}