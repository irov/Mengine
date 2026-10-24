#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"

#include "Config/Char.h"

namespace Mengine
{
    class LanguageServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LanguageService" )

    public:
        virtual const ConstString & getUserLanguage() const = 0;

    public:
        virtual bool findUserLocale( const VectorConstString & _locales, ConstString * const _locale ) const = 0;
        virtual bool lookupLanguageLocale( const Char * _language, const VectorConstString & _locales, ConstString * const _locale ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LANGUAGE_SERVICE()\
    ((Mengine::LanguageServiceInterface *)SERVICE_GET(Mengine::LanguageServiceInterface))
//////////////////////////////////////////////////////////////////////////
