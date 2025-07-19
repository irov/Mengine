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

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jclass jclass_MengineLoggerMessageParam = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineParamLoggerMessage" );

        jmethodID jmethod_MengineLoggerMessageParam_constructor = jenv->GetMethodID( jclass_MengineLoggerMessageParam, "<init>", "(Lorg/Mengine/Base/MengineTag;Ljava/lang/String;IILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V" );

        jstring jcategory = jenv->NewStringUTF( message.category );

        jclass jclass_MengineTag = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineTag" );

        jmethodID mid_MengineTag_of = jenv->GetStaticMethodID( jclass_MengineTag, "of", "(Ljava/lang/String;)Lorg/Mengine/Base/MengineTag;" );

        jobject jcategoryTag = jenv->CallStaticObjectMethod( jclass_MengineTag, mid_MengineTag_of, jcategory );

        jstring jthread = jenv->NewStringUTF( message.thread.c_str() );

        jint jlevel = message.level;
        jint jfilter = message.filter;

#ifdef MENGINE_DEBUG
        jstring jfile = jenv->NewStringUTF( message.file );
        jint jline = message.line;
        jstring jfunction = jenv->NewStringUTF( message.function );
#else
        jstring jfile = nullptr;
        jint jline = 0;
        jstring jfunction = nullptr;
#endif

        jstring jdata = jenv->NewStringUTF( message.data );

        jobject jmessage = jenv->NewObject( jclass_MengineLoggerMessageParam, jmethod_MengineLoggerMessageParam_constructor
            , jcategoryTag
            , jthread
            , jlevel
            , jfilter
            , jfile
            , jline
            , jfunction
            , jdata
        );

        jenv->DeleteLocalRef( jclass_MengineLoggerMessageParam );
        jenv->DeleteLocalRef( jclass_MengineTag );

        jenv->DeleteLocalRef( jcategoryTag );
        jenv->DeleteLocalRef( jcategory );
        jenv->DeleteLocalRef( jthread );

#ifdef MENGINE_DEBUG
        jenv->DeleteLocalRef( jfile );
        jenv->DeleteLocalRef( jfunction );
#endif
        jenv->DeleteLocalRef( jdata );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentLogger", "log", "(Lorg/Mengine/Base/MengineParamLoggerMessage;)V", jmessage );

        jenv->DeleteLocalRef( jmessage );
    }
    //////////////////////////////////////////////////////////////////////////
}
