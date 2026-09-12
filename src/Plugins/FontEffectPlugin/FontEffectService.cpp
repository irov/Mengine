#include "FontEffectService.h"

#include "Interface/PrototypeServiceInterface.h"

#include "FontEffectBase.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FontEffectService, Mengine::FontEffectService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectService::FontEffectService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectService::~FontEffectService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectInterfacePtr FontEffectService::createFontEffect( const FontEffectDesc & _desc, uint32_t _sample, const DocumentInterfacePtr & _doc )
    {
        FontEffectBasePtr fontEffect = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectBase" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fontEffect, "invalid create font effect" );

        if( fontEffect->setupDesc( _desc ) == false )
        {
            LOGGER_ERROR( "invalid create font effect" );

            return nullptr;
        }

        if( _sample != 0 )
        {
            fontEffect->setEffectSample( _sample );
        }

        return fontEffect;
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectInterfacePtr FontEffectService::createFontEffectFromFile( const ContentInterfacePtr & _content, const ConstString & _effectName, uint32_t _sample, const DocumentInterfacePtr & _doc )
    {
        FontEffectInterfacePtr fontEffect = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FontEffectFile" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fontEffect, "invalid create font effect file '%s'"
            , _effectName.c_str()
        );

        fontEffect->setContent( _content );

        UnknownFontEffectFileInterface * unknown = fontEffect->getDynamicUnknown();

        unknown->setEffectName( _effectName );

        if( _sample != 0 )
        {
            fontEffect->setEffectSample( _sample );
        }

        return fontEffect;
    }
    //////////////////////////////////////////////////////////////////////////
}
