#pragma once

#include "AppleHelpshiftInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"

#import <HelpshiftX/Helpshift.h>

namespace Mengine
{
    class AppleHelpshiftService
         : public ServiceBase<AppleHelpshiftServiceInterface>
    {
    public:
        AppleHelpshiftService();
        ~AppleHelpshiftService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void showFAQs() override;
        void showConversation() override;
        void showFAQSection( const Char * _sectionId ) override;
        void showSingleFAQ( const Char * _faqId ) override;
        void setLanguage( const Char * _language ) override;
    };
}
