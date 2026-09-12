#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UnknownInterface.h"
#include "Interface/FontEffectInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"

#include "FontEffectDesc.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FontEffectService" )

    public:
        virtual FontEffectInterfacePtr createFontEffect( const FontEffectDesc & _desc, uint32_t _sample, const DocumentInterfacePtr & _doc ) = 0;
        virtual FontEffectInterfacePtr createFontEffectFromFile( const ContentInterfacePtr & _content, const ConstString & _effectName, uint32_t _sample, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define FONTEFFECT_SERVICE()\
    ((Mengine::FontEffectServiceInterface *)SERVICE_GET(Mengine::FontEffectServiceInterface))
//////////////////////////////////////////////////////////////////////////
