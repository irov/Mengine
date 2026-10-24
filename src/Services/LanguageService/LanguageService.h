#pragma once

#include "Interface/LanguageServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class LanguageService
        : public ServiceBase<LanguageServiceInterface>
    {
        DECLARE_FACTORABLE( LanguageService );

    public:
        LanguageService();
        ~LanguageService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        const ConstString & getUserLanguage() const override;

    public:
        bool findUserLocale( const VectorConstString & _locales, ConstString * const _locale ) const override;
        bool lookupLanguageLocale( const Char * _language, const VectorConstString & _locales, ConstString * const _locale ) const override;

    protected:
        ConstString m_userLanguage;
    };
};
