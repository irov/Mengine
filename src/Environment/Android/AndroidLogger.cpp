#include "AndroidLogger.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

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

        Char buffer[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char date[256 + 1] = {'\0'};

            size_t dateSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", date, 0, 256 );
            MENGINE_UNUSED( dateSize );

            StdString::strcat_safe( buffer, date, MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, " ", MENGINE_LOGGER_MAX_MESSAGE );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            StdString::strcat_safe( buffer, "[", MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, message.category, MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, "]", MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, " ", MENGINE_LOGGER_MAX_MESSAGE );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256 + 1] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.thread, "|%s|", threadstamp, 0, 256 );
            MENGINE_UNUSED( threadstampSize );

            StdString::strcat_safe( buffer, threadstamp, MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, " ", MENGINE_LOGGER_MAX_MESSAGE );
        }

        if( message.flag & ELoggerFlag::LFLAG_FILESTAMP )
        {
            Path functionstamp = {'\0'};
            Helper::makeLoggerFunctionStamp( message.file, message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

            StdString::strcat_safe( buffer, functionstamp, MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, " ", MENGINE_LOGGER_MAX_MESSAGE );
        }

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            StdString::strcat_safe( buffer, message.function, MENGINE_LOGGER_MAX_MESSAGE );
            StdString::strcat_safe( buffer, " ", MENGINE_LOGGER_MAX_MESSAGE );
        }

        const Char * data = message.data;
        size_t data_size = message.size;

        size_t buffer_len = StdString::strlen( buffer );

        if( data_size + buffer_len >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            data_size = MENGINE_LOGGER_MAX_MESSAGE - buffer_len - 1;
        }

        StdString::strzcat_safe( buffer, data, data_size, MENGINE_LOGGER_MAX_MESSAGE );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        jclass jclass_UtilLog = Mengine_JNI_FindClass( jenv, "android/util/Log" );

        if( jclass_UtilLog == nullptr )
        {
            return;
        }

        const Char * method = nullptr;

        ELoggerLevel level = message.level;

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

        jmethodID jclass_UtilLog_method = Mengine_JNI_GetStaticMethodID( jenv, jclass_UtilLog, method, "(Ljava/lang/String;Ljava/lang/String;)I" );

        if( jclass_UtilLog_method == nullptr )
        {
            Mengine_JNI_DeleteLocalRef( jenv, jclass_UtilLog );

            return;
        }

        jstring jstring_Mengine = Mengine_JNI_NewStringUTF( jenv, "Mengine" );
        jstring jstring_buffer = Mengine_JNI_NewStringUTF( jenv, buffer );

        jint result = Mengine_JNI_CallStaticIntMethod( jenv, jclass_UtilLog, jclass_UtilLog_method, jstring_Mengine, jstring_buffer );
        MENGINE_UNUSED( result );

        Mengine_JNI_DeleteLocalRef( jenv, jstring_Mengine );
        Mengine_JNI_DeleteLocalRef( jenv, jstring_buffer );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_UtilLog );
    }
    //////////////////////////////////////////////////////////////////////////
}