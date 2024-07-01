#include "iOSProxyLogger.h"

#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#include "Kernel/AssertionUtf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSProxyLogger::iOSProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSProxyLogger::~iOSProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSProxyLogger::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSProxyLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSProxyLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        MENGINE_ASSERTION_VALIDATE_UTF8( message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( message.data, MENGINE_UNKNOWN_SIZE );

        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

        [delegate log:_record];
    }
    //////////////////////////////////////////////////////////////////////////
}
