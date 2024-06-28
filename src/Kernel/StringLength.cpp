#include "Kernel/StringLength.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t stringLength( const Char * _value )
        {
            size_t len = MENGINE_STRLEN( _value );

            return len;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t stringLength( const WChar * _value )
        {
            size_t len = MENGINE_WCSLEN( _value );

            return len;
        }
        //////////////////////////////////////////////////////////////////////////
        Char * stringFindLastOccurrenceChar( Char * const _value, Char _ch )
        {
            Char * ch_pos = MENGINE_STRRCHR( _value, _ch );

            return ch_pos;
        }
        //////////////////////////////////////////////////////////////////////////
        WChar * stringFindLastOccurrenceChar( WChar * const _value, WChar _ch )
        {
            WChar * ch_pos = MENGINE_WCSRCHR( _value, _ch );

            return ch_pos;
        }
        //////////////////////////////////////////////////////////////////////////        
    }
}