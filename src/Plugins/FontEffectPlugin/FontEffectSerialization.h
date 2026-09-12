#pragma once

#include "FontEffectDesc.h"

#include "Kernel/JSON.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool loadFontEffectDesc( const jpp::object & _json, FontEffectDesc * const _desc );
        void dumpFontEffectDesc( const FontEffectDesc & _desc, jpp::object * const _json );
        //////////////////////////////////////////////////////////////////////////
    }
}
