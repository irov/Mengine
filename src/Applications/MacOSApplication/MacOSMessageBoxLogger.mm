#import "MacOSMessageBoxLogger.h"

#import "Kernel/DebugBreak.h"

#import <AppKit/AppKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSMessageBoxLogger::MacOSMessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSMessageBoxLogger::~MacOSMessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSMessageBoxLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

#if defined(MENGINE_DEBUG)
        Helper::debuggerBreak();
#endif

        NSString * nsMessage = [NSString stringWithUTF8String:message.data];

        dispatch_async( dispatch_get_main_queue(), ^{
            NSAlert * alert = [[NSAlert alloc] init];
            alert.messageText = @"Mengine fatal error";
            alert.informativeText = nsMessage;
            alert.alertStyle = NSAlertStyleCritical;
            [alert addButtonWithTitle:@"OK"];
            [alert runModal];
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}
