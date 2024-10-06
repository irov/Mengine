#include "AssertionIdentity.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/MixinDebug.h"

#   include "Config/StdIO.h"
#   include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {        
        void assertionIdentity( const Char * _category, const Identity * _identity, const Char * _file, uint32_t _line )
        {
            UniqueId uniqueId = _identity->getUniqueIdentity();

            if( uniqueId != INVALID_UNIQUE_ID )
            {
                return;
            }

            Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
            MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "Identity '%s' has invalid uniqueId [%s]"
                , _identity->getName().c_str()
                , MENGINE_MIXIN_DEBUG_TYPE( _identity )
            );

            Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Identity" );
        }
    }
}
#endif