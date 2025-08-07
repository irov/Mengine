#include "AndroidProxyLogger.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidFragmentHelper.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Error.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidProxyLogger::AndroidProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidProxyLogger::~AndroidProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidProxyLogger::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        MENGINE_ASSERTION_VALIDATE_UTF8( message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( message.data, message.size );

        MengineJNIEnvThread * jenv = Mengine_JNI_GetEnvThread();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jclass jclass_MengineLoggerMessageParam = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineParamLoggerMessage" );

        jmethodID jmethod_MengineLoggerMessageParam_constructor = Mengine_JNI_GetMethodID( jenv, jclass_MengineLoggerMessageParam, "<init>", "(Lorg/Mengine/Base/MengineTag;Ljava/lang/String;IILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V" );

        jstring jstring_category = Mengine_JNI_NewStringUTF( jenv, message.category );

        jclass jclass_MengineTag = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineTag" );

        jmethodID mid_MengineTag_of = Mengine_JNI_GetStaticMethodID( jenv, jclass_MengineTag, "of", "(Ljava/lang/String;)Lorg/Mengine/Base/MengineTag;" );

        jobject jobject_CategoryTag = Mengine_JNI_CallStaticObjectMethod( jenv, jclass_MengineTag, mid_MengineTag_of, jstring_category );

        jstring jstring_thread = Mengine_JNI_NewStringUTF( jenv, message.thread.c_str() );

        jint jlevel = message.level;
        jint jfilter = message.filter;

#ifdef MENGINE_DEBUG
        jstring jstring_file = Mengine_JNI_NewStringUTF( jenv, message.file );
        jint jline = message.line;
        jstring jstring_function = Mengine_JNI_NewStringUTF( jenv, message.function );
#else
        jstring jstring_file = nullptr;
        jint jline = 0;
        jstring jstring_function = nullptr;
#endif

        jstring jstring_data = Mengine_JNI_NewStringUTF( jenv, message.data );

        jobject jstring_message = Mengine_JNI_NewObject( jenv, jclass_MengineLoggerMessageParam, jmethod_MengineLoggerMessageParam_constructor
            , jobject_CategoryTag
            , jstring_thread
            , jlevel
            , jfilter
            , jstring_file
            , jline
            , jstring_function
            , jstring_data
        );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_MengineLoggerMessageParam );
        Mengine_JNI_DeleteLocalRef( jenv, jclass_MengineTag );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_CategoryTag );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_category );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_thread );

#ifdef MENGINE_DEBUG
        Mengine_JNI_DeleteLocalRef( jenv, jstring_file );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_function );
#endif

        Mengine_JNI_DeleteLocalRef( jenv, jstring_data );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentLogger", "log", "(Lorg/Mengine/Base/MengineParamLoggerMessage;)V", jstring_message );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_message );
    }
    //////////////////////////////////////////////////////////////////////////
}
