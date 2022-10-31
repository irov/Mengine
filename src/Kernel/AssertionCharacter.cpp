#include "AssertionCharacter.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#include "Kernel/CharacterHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStandardCharacterSet( const Char * _value, size_t _len, const Char * _file, uint32_t _line )
        {
            for( const Char * ch = _value; ch != _value + _len; ++ch )
            {
                if( Helper::isStandardCharacterSet( *ch ) == true )
                {
                    continue;
                }

                Char msg[1024] = {'\0'};
                MENGINE_SNPRINTF( msg, 1024, "[Assert] ConstString '%.*s' has non standard char ['%u']"
                    , (int32_t)_len
                    , _value
                    , (uint32_t)*ch
                );

                Mengine::Helper::Assertion( ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Standard Character Set" );
            }
        }
    }
}
#endif