#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    int Mengine_JNI_Initialize( JNIEnv * _env );
    int Mengine_JNI_SetupThread();

    JNIEnv * Mengine_JNI_GetEnv();

    jboolean Mengine_JNI_IsInstanceOf( JNIEnv * _jenv, jobject _jobject, jclass _jclass );

    jclass Mengine_JNI_GetClassBoolean( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassCharacter( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassInteger( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassLong( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassFloat( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassDouble( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassString( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassException( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassList( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassMap( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassSet( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassArrayList( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassHashMap( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassMapEntry( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassIterator( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassRect( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassJSONObject( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassJSONArray( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassClass( JNIEnv * _jenv );
    jclass Mengine_JNI_GetClassClassLoader( JNIEnv * _jenv );

    jclass Mengine_JNI_GetObjectClass( JNIEnv * _jenv, jobject _jobject );

    void Mengine_JNI_DeleteLocalRef( JNIEnv * _jenv, jobject _jobject );
    jobject Mengine_JNI_NewGlobalRef( JNIEnv * _jenv, jobject _jobject );
    void Mengine_JNI_DeleteGlobalRef( JNIEnv * _jenv, jobject _jobject );

    int Mengine_JNI_ExceptionCheck( JNIEnv * _jenv );
    jthrowable Mengine_JNI_ExceptionOccurred( JNIEnv * _jenv );
    void Mengine_JNI_ExceptionClear( JNIEnv * _jenv );

    int Mengine_JNI_PushLocalFrame( JNIEnv * _jenv, jint _capacity );
    jobject Mengine_JNI_PopLocalFrame( JNIEnv * _jenv, jobject _result );

    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const Char * _name );

    jmethodID Mengine_JNI_GetMethodID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jmethodID Mengine_JNI_GetStaticMethodID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature );

    jfieldID Mengine_JNI_GetFieldID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jint Mengine_JNI_GetIntField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jboolean Mengine_JNI_GetBooleanField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jchar Mengine_JNI_GetCharField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jlong Mengine_JNI_GetLongField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jfloat Mengine_JNI_GetFloatField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jdouble Mengine_JNI_GetDoubleField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );
    jobject Mengine_JNI_GetObjectField( JNIEnv * _jenv, jobject _jobject, jfieldID _fieldID );

    jfieldID Mengine_JNI_GetStaticFieldID( JNIEnv * _jenv, jclass _jclass, const Char * _name, const Char * _signature );
    jobject Mengine_JNI_GetStaticObjectField( JNIEnv * _jenv, jclass _jclass, jfieldID _fieldID );

    void Mengine_JNI_CallVoidMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    void Mengine_JNI_CallVoidMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    void Mengine_JNI_CallVoidMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jboolean Mengine_JNI_CallBooleanMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jboolean Mengine_JNI_CallBooleanMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jboolean Mengine_JNI_CallBooleanMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jchar Mengine_JNI_CallCharMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jchar Mengine_JNI_CallCharMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jchar Mengine_JNI_CallCharMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jint Mengine_JNI_CallIntMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jint Mengine_JNI_CallIntMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jint Mengine_JNI_CallIntMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jlong Mengine_JNI_CallLongMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jlong Mengine_JNI_CallLongMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jlong Mengine_JNI_CallLongMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jfloat Mengine_JNI_CallFloatMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jfloat Mengine_JNI_CallFloatMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jfloat Mengine_JNI_CallFloatMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jdouble Mengine_JNI_CallDoubleMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jdouble Mengine_JNI_CallDoubleMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jdouble Mengine_JNI_CallDoubleMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_CallObjectMethod( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, ... );
    jobject Mengine_JNI_CallObjectMethodV( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, va_list _args );
    jobject Mengine_JNI_CallObjectMethodA( JNIEnv * _jenv, jobject _jobject, jmethodID _methodID, const jvalue * _args );

    void Mengine_JNI_CallStaticVoidMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    void Mengine_JNI_CallStaticVoidMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    void Mengine_JNI_CallStaticVoidMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jint Mengine_JNI_CallStaticIntMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jint Mengine_JNI_CallStaticIntMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jint Mengine_JNI_CallStaticIntMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jlong Mengine_JNI_CallStaticLongMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jlong Mengine_JNI_CallStaticLongMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jlong Mengine_JNI_CallStaticLongMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jboolean Mengine_JNI_CallStaticBooleanMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jboolean Mengine_JNI_CallStaticBooleanMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jboolean Mengine_JNI_CallStaticBooleanMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_CallStaticObjectMethod( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jobject Mengine_JNI_CallStaticObjectMethodV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );
    jobject Mengine_JNI_CallStaticObjectMethodA( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, const jvalue * _args );

    jobject Mengine_JNI_NewObject( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, ... );
    jobject Mengine_JNI_NewObjectV( JNIEnv * _jenv, jclass _jclass, jmethodID _methodID, va_list _args );

    jstring Mengine_JNI_NewStringUTF( JNIEnv * _jenv, const Char * _str );
    const Char * Mengine_JNI_GetStringUTFChars( JNIEnv * _jenv, jstring _jstring, jboolean * const _isCopy );
    void Mengine_JNI_ReleaseStringUTFChars( JNIEnv * _jenv, jstring _jstring, const Char * _cstr );
    jsize Mengine_JNI_GetStringUTFLength( JNIEnv * _jenv, jstring _jstring );

    jsize Mengine_JNI_GetArrayLength( JNIEnv * _jenv, jobjectArray _jarray );
    jobject Mengine_JNI_GetObjectArrayElement( JNIEnv * _jenv, jobjectArray _jarray, jsize _index );

    jbyteArray Mengine_JNI_NewByteArray( JNIEnv * _jenv, jsize _length );
    void Mengine_JNI_SetByteArrayRegion( JNIEnv * _jenv, jbyteArray _jarray, jsize _start, jsize _len, const jbyte * _buf );
    void Mengine_JNI_ReleaseByteArrayElements( JNIEnv * _jenv, jbyteArray _jarray, jbyte * _elements, jint _mode );
    jbyte * Mengine_JNI_GetByteArrayElements( JNIEnv * _jenv, jbyteArray _jarray, jboolean * const _isCopy );

    jobject Mengine_JNI_NewDirectByteBuffer( JNIEnv * _jenv, void * _address, jlong _capacity );
    void * Mengine_JNI_GetDirectBufferAddress( JNIEnv * _jenv, jobject _jbuffer );

    jclass Mengine_JNI_GetClassMengineCallback( JNIEnv * _jenv );

    jclass Mengine_JNI_GetClassApplication( JNIEnv * _jenv );
    jobject Mengine_JNI_GetObjectApplication( JNIEnv * _jenv );
    jmethodID Mengine_JNI_GetMethodApplication( JNIEnv * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassActivity( JNIEnv * _jenv );
    jobject Mengine_JNI_GetObjectActivity( JNIEnv * _jenv );
    jmethodID Mengine_JNI_GetMethodActivity( JNIEnv * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassFragment( JNIEnv * _jenv, const Char * _fragment );
    jobject Mengine_JNI_GetObjectFragment( JNIEnv * _jenv, const Char * _fragment );
    jmethodID Mengine_JNI_GetMethodFragment( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_LoadClass( JNIEnv * _jenv, const Char * _class );

    AAssetManager * Mengine_JNI_GetAssetManagerFromJava( JNIEnv * _jenv, jobject _assetManager );
    ANativeWindow * Mengine_JNI_ANativeWindow_fromSurface( JNIEnv * _jenv, jobject _surface );
}