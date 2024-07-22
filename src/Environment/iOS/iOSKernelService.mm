#include "iOSKernelService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#import "Environment/iOS/iOSProxyLogger.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdString.h"

#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( iOSKernelService, Mengine::iOSKernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSKernelService::iOSKernelService()
        : m_mailCompose( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSKernelService::~iOSKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSKernelService::_initializeService()
    {
        iOSProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<iOSProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        if( LOGGER_SERVICE()
            ->registerLogger( proxyLogger ) == true )
        {
            m_proxyLogger = proxyLogger;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSKernelService::_finalizeService()
    {
        if( m_proxyLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_proxyLogger );

            m_proxyLogger = nullptr;
        }
        
        m_mailCompose = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSKernelService::openUrlInDefaultBrowser( const Char * _url )
    {
        NSURL * ns_url = [NSURL URLWithString:@(_url)];
        
        if( [[UIApplication sharedApplication] canOpenURL:ns_url] == NO )
        {
            return false;
        }
        
        [[UIApplication sharedApplication] openURL:ns_url options:@{} completionHandler:^(BOOL success) {
            //ToDo callback
        }];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSKernelService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        UIViewController * view = [iOSDetail getRootViewController];
        
        if( [iOSMailCompose canSendMail] == NO )
        {
            [iOSDetail alertWithViewController:view title:@"Yikes." message:@"Log into your mailbox using the standard Mail app" callback:^{}];
            
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
