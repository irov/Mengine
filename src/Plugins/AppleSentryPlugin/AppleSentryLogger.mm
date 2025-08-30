#include "AppleSentryLogger.h"

#include "Interface/LoggerRecordInterface.h"
#include "Kernel/LoggerMessage.h"

#import <Sentry/Sentry.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSentryLogger::AppleSentryLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSentryLogger::~AppleSentryLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryLogger::_initializeLogger()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    static SentryLevel s_getSentryLevel( ELoggerLevel _level )
    {
        switch( _level )
        {
        case LM_FATAL:
            return kSentryLevelFatal;
        case LM_ERROR:
            return kSentryLevelError;
        case LM_WARNING:
            return kSentryLevelWarning;
        case LM_INFO:
        case LM_MESSAGE:
        case LM_MESSAGE_RELEASE:
            return kSentryLevelInfo;
        case LM_DEBUG:
        case LM_VERBOSE:
            return kSentryLevelDebug;
        default:
            break;
        }

        return kSentryLevelInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        NSString * nsMessage = [NSString stringWithUTF8String:message.data];
        NSString * nsCategory = [NSString stringWithUTF8String:message.category];
        NSString * nsThread = [NSString stringWithUTF8String:message.thread.c_str()];

        NSString * nsFile = nil;
        if( message.file != nullptr && message.file[0] != '\0' )
        {
            nsFile = [NSString stringWithUTF8String:message.file];
        }

        NSString * nsFunction = nil;
        if( message.function != nullptr && message.function[0] != '\0' )
        {
            nsFunction = [NSString stringWithUTF8String:message.function];
        }

        SentryLevel level = s_getSentryLevel( message.level );

        [SentrySDK captureMessage:nsMessage withScope:^(SentryScope * _Nonnull scope) {
            scope.level = level;
            [scope setExtraValue:nsCategory forKey:@"log.category"];
            [scope setExtraValue:nsThread forKey:@"log.thread"];

            if( nsFile != nil )
            {
                [scope setExtraValue:nsFile forKey:@"log.file"];

                if( message.line != 0 )
                {
                    [scope setExtraValue:@(message.line) forKey:@"log.line"];
                }
            }

            if( nsFunction != nil )
            {
                [scope setExtraValue:nsFunction forKey:@"log.function"];
            }
        }];
    }
    //////////////////////////////////////////////////////////////////////////
}
