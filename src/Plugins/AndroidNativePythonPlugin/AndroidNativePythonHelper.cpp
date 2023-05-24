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
                jmethodID methodValue = _jenv->GetMethodID( jclass_Boolean, "booleanValue", "()Z" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()C'" );

                jboolean value = _jenv->CallBooleanMethod( _obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                py_value = _kernel->ret_bool( value );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_Character ) == JNI_TRUE )
            {
                jmethodID methodValue = _jenv->GetMethodID( jclass_Character, "charValue", "()C" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

                jchar value = _jenv->CallCharMethod( _obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                Char value_str[2] = {(Char)value, '\0'};
                py_value = _kernel->string_from_char_size( value_str, 1 );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_Integer ) == JNI_TRUE )
            {
                jmethodID methodValue = _jenv->GetMethodID( jclass_Integer, "intValue", "()I" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

                jint value = _jenv->CallIntMethod( _obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                py_value = _kernel->ptr_int32( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Long ) == JNI_TRUE )
            {
                jmethodID methodValue = _jenv->GetMethodID( jclass_Long, "longValue", "()J" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

                jlong value = _jenv->CallLongMethod(_obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                py_value = _kernel->ptr_int64( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Float ) == JNI_TRUE )
            {
                jmethodID methodValue = _jenv->GetMethodID( jclass_Float, "floatValue", "()F" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

                jfloat value = _jenv->CallFloatMethod( _obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                py_value = _kernel->ptr_float( value );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Double ) == JNI_TRUE )
            {
                jmethodID methodValue = _jenv->GetMethodID( jclass_Double, "doubleValue", "()D" );

                MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

                jfloat value = _jenv->CallDoubleMethod( _obj, methodValue );

                Helper::jEnvExceptionCheck( _jenv );

                py_value = _kernel->ptr_float( value );
            }
            else if( _jenv->IsInstanceOf(_obj, jclass_String ) == JNI_TRUE )
            {
                const Char * obj_str = _jenv->GetStringUTFChars( (jstring)_obj, nullptr );

                py_value = _kernel->string_from_char( obj_str );

                _jenv->ReleaseStringUTFChars( (jstring)_obj, obj_str );
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_List ) == JNI_TRUE )
            {
                jmethodID List_size = _jenv->GetMethodID( jclass_List, "size", "()I");

                MENGINE_ASSERTION_FATAL( List_size != nullptr, "invalid get android method 'java/lang/List [size] ()I'" );

                jmethodID List_get = _jenv->GetMethodID( jclass_List, "get", "(I)Ljava/lang/Object;");

                MENGINE_ASSERTION_FATAL( List_get != nullptr, "invalid get android method 'java/lang/List [get] (I)Ljava/lang/Object;'" );

                int list_size = _jenv->CallIntMethod( _obj, List_size );

                Helper::jEnvExceptionCheck( _jenv );

                PyObject * py_list = _kernel->list_new( list_size );

                for( jsize index = 0; index != list_size; ++index )
                {
                    jobject list_obj = _jenv->CallObjectMethod( _obj, List_get, index );

                    Helper::jEnvExceptionCheck( _jenv );

                    PyObject * py_obj = Helper::androidNativePythonMakePyObject( _kernel, _jenv, list_obj, _doc );

                    MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                    _kernel->list_setitem( py_list, index, py_obj );

                    _kernel->decref( py_obj );

                    _jenv->DeleteLocalRef( list_obj );
                }

                py_value = py_list;
            }
            else if( _jenv->IsInstanceOf( _obj, jclass_Map ) == JNI_TRUE )
            {
                PyObject * py_dict = _kernel->dict_new();

                jclass jclass_Iterator = _jenv->FindClass( "java/util/Iterator" );
                jclass jclass_MapEntry = _jenv->FindClass( "java/util/Map$Entry" );

                jmethodID jmethodID_Map_entrySet = _jenv->GetMethodID( jclass_Map, "entrySet", "()Ljava/util/Set;" );
                jmethodID jmethodID_Set_iterator = _jenv->GetMethodID( jclass_Set, "iterator", "()Ljava/util/Iterator;" );
                jmethodID jmethodID_Iterator_hasNext = _jenv->GetMethodID( jclass_Iterator, "hasNext", "()Z" );
                jmethodID jmethodID_Iterator_next = _jenv->GetMethodID( jclass_Iterator, "next", "()Ljava/lang/Object;" );
                jmethodID jmethodID_MapEntry_getKey = _jenv->GetMethodID( jclass_MapEntry, "getKey", "()Ljava/lang/Object;" );
                jmethodID jmethodID_MapEntry_getValue = _jenv->GetMethodID( jclass_MapEntry, "getValue", "()Ljava/lang/Object;" );

                jobject jset = _jenv->CallObjectMethod( _obj, jmethodID_Map_entrySet );
                Helper::jEnvExceptionCheck( _jenv );

                jobject jset_iterator = _jenv->CallObjectMethod( jset, jmethodID_Set_iterator );
                Helper::jEnvExceptionCheck( _jenv );

                jboolean hasNext = _jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );
                Helper::jEnvExceptionCheck( _jenv );

                while( hasNext == JNI_TRUE )
                {
                    jobject jentry = _jenv->CallObjectMethod( jset_iterator, jmethodID_Iterator_next );
                    Helper::jEnvExceptionCheck( _jenv );

                    jobject jkey = _jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getKey );
                    Helper::jEnvExceptionCheck( _jenv );

                    jobject jvalue = (jstring)_jenv->CallObjectMethod( jentry, jmethodID_MapEntry_getValue );
                    Helper::jEnvExceptionCheck( _jenv );

                    PyObject * py_key = Helper::androidNativePythonMakePyObject( _kernel, _jenv, jkey, _doc );
                    PyObject * py_value = Helper::androidNativePythonMakePyObject( _kernel, _jenv, jvalue, _doc );

                    _kernel->dict_set( py_dict, py_key, py_value );

                    _kernel->decref( py_key );
                    _kernel->decref( py_value );

                    _jenv->DeleteLocalRef( jkey );
                    _jenv->DeleteLocalRef( jvalue );
                    _jenv->DeleteLocalRef( jentry );

                    hasNext = _jenv->CallBooleanMethod( jset_iterator, jmethodID_Iterator_hasNext );
                    Helper::jEnvExceptionCheck( _jenv );
                }

                _jenv->DeleteLocalRef( jset_iterator );
                _jenv->DeleteLocalRef( jset );

                _jenv->DeleteLocalRef( jclass_Iterator );
                _jenv->DeleteLocalRef( jclass_MapEntry );

                Helper::jEnvExceptionCheck( _jenv );

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

                jmethodID jmethodID_getClass = _jenv->GetMethodID( cls_obj, "getClass", "()Ljava/lang/Class;" );

                MENGINE_ASSERTION_FATAL( jmethodID_getClass != nullptr, "invalid get android method 'getClass()Ljava/lang/Class;'" );

                jobject obj_class = _jenv->CallObjectMethod( _obj, jmethodID_getClass );

                Helper::jEnvExceptionCheck( _jenv );

                jclass cls_class = _jenv->GetObjectClass( obj_class );

                jmethodID jmethodID_getName = _jenv->GetMethodID( cls_class, "getName", "()Ljava/lang/String;" );

                MENGINE_ASSERTION_FATAL( jmethodID_getName != nullptr, "invalid get android method 'getName()Ljava/lang/String;'" );

                jstring obj_class_name = (jstring)_jenv->CallObjectMethod( obj_class, jmethodID_getName );

                Helper::jEnvExceptionCheck( _jenv );

                const Char * obj_class_name_str = _jenv->GetStringUTFChars( obj_class_name, nullptr );

                LOGGER_ERROR( "unsupported java argument type '%s'"
                    , obj_class_name_str
                );

                _jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
                _jenv->DeleteLocalRef( obj_class_name );

                _jenv->DeleteLocalRef( obj_class );
                _jenv->DeleteLocalRef( cls_class );
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