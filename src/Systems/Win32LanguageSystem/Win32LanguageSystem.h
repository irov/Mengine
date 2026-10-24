#pragma once

#include "Interface/LanguageSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32LanguageSystem
        : public ServiceBase<LanguageSystemInterface>
    {
        DECLARE_FACTORABLE( Win32LanguageSystem );

    public:
        Win32LanguageSystem();
        ~Win32LanguageSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool getUserLanguage( Char * const _userLanguage, size_t _capacity ) const override;
    };
};
