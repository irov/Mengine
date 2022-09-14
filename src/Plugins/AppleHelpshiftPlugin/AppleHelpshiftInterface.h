#pragma once

#include "Interface/ServiceInterface.h"

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
        virtual void showFAQSection( const Char * _sectionId ) = 0;
        virtual void showSingleFAQ( const Char * _faqId ) = 0;
        virtual void setLanguage( const Char * _language ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_HELPSHIFT_SERVICE()\
    ((Mengine::AppleHelpshiftServiceInterface *)SERVICE_GET(Mengine::AppleHelpshiftServiceInterface))
//////////////////////////////////////////////////////////////////////////
