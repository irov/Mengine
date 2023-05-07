#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t stringLength( const Char * _value );
        size_t stringLength( const WChar * _value );
        Char * stringFindLastOccurrenceChar( Char * const _value, Char _ch );
        WChar * stringFindLastOccurrenceChar( WChar * const _value, WChar _ch );
        template<class T>
        T stringTerminalChar();
        //////////////////////////////////////////////////////////////////////////
        template<>
        Char stringTerminalChar();
        //////////////////////////////////////////////////////////////////////////
        template<>
        WChar stringTerminalChar();
        //////////////////////////////////////////////////////////////////////////
    }
}
