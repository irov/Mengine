#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHelpshiftServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleHelpshiftService" )
        
    public:
        virtual void showFAQs() = 0;
        virtual void showConversation() = 0;
        virtual void showFAQSection(const char* setionId) = 0;
        virtual void showSingleFAQ(const char* faqId) = 0;
        virtual void setLanguage( const char * lang) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_HELPSHIFT_SERVICE()\
    ((Mengine::AppleHelpshiftServiceInterface *)SERVICE_GET(Mengine::AppleHelpshiftServiceInterface))
//////////////////////////////////////////////////////////////////////////
