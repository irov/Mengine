#include "iOSAlertLogger.h"

#include "Kernel/DebugBreak.h"

#import "Environment/iOS/iOSDetail.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSAlertLogger::iOSAlertLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAlertLogger::~iOSAlertLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAlertLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

#if defined(MENGINE_DEBUG)
        Helper::debuggerBreak();
#endif

        NSString * nsMessage = @(message.data);

        dispatch_async( dispatch_get_main_queue(), ^{
            [iOSDetail showOkAlertWithTitle:@"Mengine fatal error"
                                    message:nsMessage
                                         ok:^{}];
        });
    }
    //////////////////////////////////////////////////////////////////////////
}
