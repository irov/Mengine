#include "AndroidLogger.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::~AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        MENGINE_ASSERTION_VALIDATE_UTF8( message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( message.data, MENGINE_UNKNOWN_SIZE );

        if( (message.filter & Mengine::LFILTER_ANDROID) == Mengine::LFILTER_ANDROID )
        {
            return;
        }

        ELoggerLevel level = message.level;

        Char buffer[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char date[256 + 1] = {'\0'};

            size_t dateSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", date, 0, 256 );
            MENGINE_UNUSED( dateSize );

            StdString::strcat( buffer, date );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            const Char * category = message.category;

            StdString::strcat( buffer, "[" );
            StdString::strcat( buffer, category );
            StdString::strcat( buffer, "]" );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256 + 1] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );
            MENGINE_UNUSED( threadstampSize );

            StdString::strcat( buffer, threadstamp );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH + 1] = {'\0'};

            const Char * function = message.function;
            int32_t line = message.line;
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( function, line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            MENGINE_UNUSED( functionstampSize );

            StdString::strcat( buffer, functionstamp );
            StdString::strcat( buffer, " " );
        }

        const Char * data = message.data;
        size_t data_size = StdString::strlen( data );

        size_t message_stamplen = StdString::strlen( buffer );

        if( data_size + message_stamplen >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            data_size = MENGINE_LOGGER_MAX_MESSAGE - message_stamplen - 1;
        }

        StdString::strncat( buffer, data, data_size );

        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidEnvExceptionCheck( jenv );

        jclass jclass_MengineLog = jenv->FindClass( "android/util/Log" );

        if( jclass_MengineLog == nullptr )
        {
            return;
        }

        const Char * method = nullptr;

        switch( level )
        {
        case LM_SILENT:
            return;
        case LM_FATAL:
            method = "wtf";
            break;
        case LM_MESSAGE_RELEASE:
            method = "w";
            break;
        case LM_ERROR:
            method = "e";
            break;
        case LM_WARNING:
            method = "w";
            break;
        case LM_MESSAGE:
            method = "i";
            break;
        case LM_INFO:
            method = "i";
            break;
        case LM_DEBUG:
            method = "d";
            break;
        case LM_VERBOSE:
            method = "v";
            break;
        default:
            return;
        }

        jmethodID jclass_Log_method = jenv->GetStaticMethodID( jclass_MengineLog, method, "(Ljava/lang/String;Ljava/lang/String;)I" );

        if( jclass_Log_method == nullptr )
        {
            return;
        }

        jstring jstring_Mengine = jenv->NewStringUTF( "Mengine" );
        jstring jstring_buffer = jenv->NewStringUTF( buffer );

        jint result = jenv->CallStaticIntMethod( jclass_MengineLog, jclass_Log_method, jstring_Mengine, jstring_buffer );
        MENGINE_UNUSED( result );

        jenv->DeleteLocalRef( jstring_Mengine );
        jenv->DeleteLocalRef( jstring_buffer );

        jenv->DeleteLocalRef( jclass_MengineLog );
    }
    //////////////////////////////////////////////////////////////////////////
}
