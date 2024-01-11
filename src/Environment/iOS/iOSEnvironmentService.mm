#include "iOSEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"

#import "Environment/iOS/iOSDetail.h"

#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( iOSEnvironmentService, Mengine::iOSEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::iOSEnvironmentService()
        : m_mailCompose(nil)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSEnvironmentService::~iOSEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSEnvironmentService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        UIViewController * view = Helper::iOSGetRootViewController();
        
        if( [iOSMailCompose canSendMail] == NO )
        {
            Helper::iOSAlert( view, @"Yikes.", @"Log into your mailbox using the standard Mail app" );
            
            return false;
        }
        
        iOSMailCompose * mailCompose = [[iOSMailCompose alloc] initWithViewController:view email:@(_email) subject:@(_subject) message:@(_body) completion:^{
            this->m_mailCompose = nullptr;
        }];

        m_mailCompose = mailCompose;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
