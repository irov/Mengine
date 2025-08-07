#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    int Mengine_JNI_Initialize( JNIEnv * _env );
    int Mengine_JNI_SetupThread();

    struct MengineJNIEnvThread {
        JNIEnv * env;

        jclass jclass_Boolean;
        jclass jclass_Character;
        jclass jclass_Integer;
        jclass jclass_Long;
        jclass jclass_Float;
        jclass jclass_Double;
        jclass jclass_String;
        jclass jclass_Exception;
        jclass jclass_List;
        jclass jclass_Map;
        jclass jclass_Set;
        jclass jclass_ArrayList;
        jclass jclass_HashMap;
        jclass jclass_MapEntry;
        jclass jclass_Iterator;
        jclass jclass_Rect;
        jclass jclass_JSONObject;
        jclass jclass_JSONArray;
        jclass jclass_Class;
        jclass jclass_ClassLoader;

        jclass jclass_MengineCallback;
        jclass jclass_MengineApplication;
        jclass jclass_MengineActivity;
    };

    MengineJNIEnvThread * Mengine_JNI_GetEnvThread();

    jboolean Mengine_JNI_IsInstanceOf( MengineJNIEnvThread * _jenv, jobject _jobject, jclass _jclass );

    jclass Mengine_JNI_GetClassBoolean( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassCharacter( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassInteger( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassLong( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassFloat( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassDouble( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassString( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassException( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassList( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassMap( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassSet( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassArrayList( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassHashMap( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassMapEntry( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassIterator( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassRect( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassJSONObject( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassJSONArray( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassClass( MengineJNIEnvThread * _jenv );
    jclass Mengine_JNI_GetClassClassLoader( MengineJNIEnvThread * _jenv );

    jclass Mengine_JNI_GetObjectClass( MengineJNIEnvThread * _jenv, jobject _jobject );

    void Mengine_JNI_DeleteLocalRef( MengineJNIEnvThread * _jenv, jobject _jobject );
    jobject Mengine_JNI_NewGlobalRef( MengineJNIEnvThread * _jenv, jobject _jobject );
    void Mengine_JNI_DeleteGlobalRef( MengineJNIEnvThread * _jenv, jobject _jobject );

    int Mengine_JNI_ExceptionCheck( MengineJNIEnvThread * _jenv );
    jthrowable Mengine_JNI_ExceptionOccurred( MengineJNIEnvThread * _jenv );
    void Mengine_JNI_ExceptionClear( MengineJNIEnvThread * _jenv );

    int Mengine_JNI_PushLocalFrame( MengineJNIEnvThread * _jenv, jint _capacity );
    jobject Mengine_JNI_PopLocalFrame( MengineJNIEnvThread * _jenv, jobject _result );

    jclass Mengine_JNI_FindClass( MengineJNIEnvThread * _jenv, const Char * _name );

    jmethodID Mengine_JNI_GetMethodID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jmethodID Mengine_JNI_GetStaticMethodID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature );

    jfieldID Mengine_JNI_GetFieldID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jint Mengine_JNI_GetIntField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jboolean Mengine_JNI_GetBooleanField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jchar Mengine_JNI_GetCharField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jlong Mengine_JNI_GetLongField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jfloat Mengine_JNI_GetFloatField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jdouble Mengine_JNI_GetDoubleField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );
    jobject Mengine_JNI_GetObjectField( MengineJNIEnvThread * _jenv, jobject _jobject, jfieldID _fieldID );

    jfieldID Mengine_JNI_GetStaticFieldID( MengineJNIEnvThread * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jobject Mengine_JNI_GetStaticObjectField( MengineJNIEnvThread * _jenv, jclass _jclass, jfieldID _fieldID );

    void Mengine_JNI_CallVoidMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    void Mengine_JNI_CallVoidMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    void Mengine_JNI_CallVoidMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jboolean Mengine_JNI_CallBooleanMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jboolean Mengine_JNI_CallBooleanMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jboolean Mengine_JNI_CallBooleanMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jchar Mengine_JNI_CallCharMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jchar Mengine_JNI_CallCharMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jchar Mengine_JNI_CallCharMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jint Mengine_JNI_CallIntMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jint Mengine_JNI_CallIntMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jint Mengine_JNI_CallIntMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jlong Mengine_JNI_CallLongMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jlong Mengine_JNI_CallLongMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jlong Mengine_JNI_CallLongMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jfloat Mengine_JNI_CallFloatMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jfloat Mengine_JNI_CallFloatMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jfloat Mengine_JNI_CallFloatMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jdouble Mengine_JNI_CallDoubleMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jdouble Mengine_JNI_CallDoubleMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jdouble Mengine_JNI_CallDoubleMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_CallObjectMethod( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jobject Mengine_JNI_CallObjectMethodV( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jobject Mengine_JNI_CallObjectMethodA( MengineJNIEnvThread * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    void Mengine_JNI_CallStaticVoidMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    void Mengine_JNI_CallStaticVoidMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    void Mengine_JNI_CallStaticVoidMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jint Mengine_JNI_CallStaticIntMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jint Mengine_JNI_CallStaticIntMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jint Mengine_JNI_CallStaticIntMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jlong Mengine_JNI_CallStaticLongMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jlong Mengine_JNI_CallStaticLongMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jlong Mengine_JNI_CallStaticLongMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jboolean Mengine_JNI_CallStaticBooleanMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jboolean Mengine_JNI_CallStaticBooleanMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jboolean Mengine_JNI_CallStaticBooleanMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_CallStaticObjectMethod( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jobject Mengine_JNI_CallStaticObjectMethodV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jobject Mengine_JNI_CallStaticObjectMethodA( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_NewObject( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jobject Mengine_JNI_NewObjectV( MengineJNIEnvThread * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );

    jstring Mengine_JNI_NewStringUTF( MengineJNIEnvThread * _jenv, const Char * _str );
    const Char * Mengine_JNI_GetStringUTFChars( MengineJNIEnvThread * _jenv, jstring _jstring, jboolean * const _isCopy );
    void Mengine_JNI_ReleaseStringUTFChars( MengineJNIEnvThread * _jenv, jstring _jstring, const Char * _cstr );
    jsize Mengine_JNI_GetStringLength( MengineJNIEnvThread * _jenv, jstring _jstring );

    jsize Mengine_JNI_GetArrayLength( MengineJNIEnvThread * _jenv, jobjectArray _jarray );
    jobject Mengine_JNI_GetObjectArrayElement( MengineJNIEnvThread * _jenv, jobjectArray _jarray, jsize _index );

    jbyteArray Mengine_JNI_NewByteArray( MengineJNIEnvThread * _jenv, jsize _length );
    void Mengine_JNI_SetByteArrayRegion( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jsize _start, jsize _len, const jbyte * _buf );
    void Mengine_JNI_ReleaseByteArrayElements( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jbyte * _elements, jint _mode );
    jbyte * Mengine_JNI_GetByteArrayElements( MengineJNIEnvThread * _jenv, jbyteArray _jarray, jboolean * const _isCopy );

    jobject Mengine_JNI_NewDirectByteBuffer( MengineJNIEnvThread * _jenv, void * _address, jlong _capacity );
    void * Mengine_JNI_GetDirectBufferAddress( MengineJNIEnvThread * _jenv, jobject _jbuffer );

    jclass Mengine_JNI_GetClassMengineCallback( MengineJNIEnvThread * _jenv );

    jclass Mengine_JNI_GetClassApplication( MengineJNIEnvThread * _jenv );
    jobject Mengine_JNI_GetObjectApplication( MengineJNIEnvThread * _jenv );
    jmethodID Mengine_JNI_GetMethodApplication( MengineJNIEnvThread * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassActivity( MengineJNIEnvThread * _jenv );
    jobject Mengine_JNI_GetObjectActivity( MengineJNIEnvThread * _jenv );
    jmethodID Mengine_JNI_GetMethodActivity( MengineJNIEnvThread * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassFragment( MengineJNIEnvThread * _jenv, const Char * _fragment );
    jobject Mengine_JNI_GetObjectFragment( MengineJNIEnvThread * _jenv, const Char * _fragment );
    jmethodID Mengine_JNI_GetMethodFragment( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_LoadClass( MengineJNIEnvThread * _jenv, const Char * _class );

    AAssetManager * Mengine_JNI_GetAssetManagerFromJava( MengineJNIEnvThread * _jenv, jobject _assetManager );
    ANativeWindow * Mengine_JNI_ANativeWindow_fromSurface( MengineJNIEnvThread * _jenv, jobject _surface );
}