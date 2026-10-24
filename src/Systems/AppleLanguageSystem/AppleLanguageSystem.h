#pragma once

#include "Interface/LanguageSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleLanguageSystem
        : public ServiceBase<LanguageSystemInterface>
    {
        DECLARE_FACTORABLE( AppleLanguageSystem );

    public:
        AppleLanguageSystem();
        ~AppleLanguageSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool getUserLanguage( Char * const _userLanguage, size_t _capacity ) const override;
    };
};
