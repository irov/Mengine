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
        void showFAQSection(const char* setionId) override;
        void showSingleFAQ(const char* faqId) override;
        void setLanguage( const char * lang) override;
    };
}
