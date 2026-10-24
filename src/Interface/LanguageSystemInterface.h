#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    class LanguageSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LanguageSystem" )

    public:
        virtual bool getUserLanguage( Char * const _userLanguage, size_t _capacity ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LANGUAGE_SYSTEM()\
    ((Mengine::LanguageSystemInterface *)SERVICE_GET(Mengine::LanguageSystemInterface))
//////////////////////////////////////////////////////////////////////////
