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
        if( [iOSMailCompose canSendMail] == NO )
        {
            return false;
        }
        
        UIViewController * view = Helper::iOSGetRootViewController();
        
        iOSMailCompose * mailCompose = [[iOSMailCompose alloc] initWithViewController:view email:@(_email) subject:@(_subject) message:@(_body) completion:^{
            this->m_mailCompose = nullptr;
        }];

        m_mailCompose = mailCompose;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
