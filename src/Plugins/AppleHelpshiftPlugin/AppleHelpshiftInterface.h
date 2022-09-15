#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHelpshiftProviderInterface
        : public ServantInterface
    {
    public:
        virtual void sessionStarted() = 0;
        virtual void sessionEnded() = 0;
        virtual void receivedUnreadMessage(int count, int countInCache) = 0;
        virtual void conversationStatus(const Char * issueId,const Char *  publishId,bool issueOpen) = 0;
        virtual void eventWidgetToggle(bool visible) = 0;
        virtual void eventMessageAdd(const Char * body,const Char * type) = 0;
        virtual void converstationStart(const Char * text) = 0;
        virtual void eventCSATSubmit(const Char * rating, const Char * feedback) = 0;
        virtual void converstationEnded() = 0;
        virtual void converstationRejected() = 0;
        virtual void converstationResolved() = 0;
        virtual void converstationReopen() = 0;
        
        virtual void authenticationInvalidAuthToken() = 0;
        virtual void authenticationTokenNotProvided() = 0;
        virtual void authenticationUnknownError() = 0;
    
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHelpshiftProviderInterface> AppleHelpshiftProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleHelpshiftServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleHelpshiftService" )
        
    public:
        virtual void setProvider( const AppleHelpshiftProviderInterfacePtr & _provider ) = 0;
        virtual const AppleHelpshiftProviderInterfacePtr & getProvider() const = 0;

        
    public:
        virtual void showFAQs() = 0;
        virtual void showConversation() = 0;
        virtual void showFAQSection( const Char * _sectionId ) = 0;
        virtual void showSingleFAQ( const Char * _faqId ) = 0;
        virtual void setLanguage( const Char * _language ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_HELPSHIFT_SERVICE()\
    ((Mengine::AppleHelpshiftServiceInterface *)SERVICE_GET(Mengine::AppleHelpshiftServiceInterface))
//////////////////////////////////////////////////////////////////////////
