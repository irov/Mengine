#include "AssertionUtf8.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/UnicodeHelper.h"

#   include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {        
        void assertionValidateUtf8( const Char * _category, const Utf8 * _value, size_t _len, const Char * _file, int32_t _line, const Char * _function )
        {
            if( _len == MENGINE_UNKNOWN_SIZE )
            {
                _len = StdString::strlen( _value );
            }

            if( Helper::utf8Validate( _value, _value + _len, nullptr ) == true )
            {
                return;
            }

            Char replaceMessage[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
            const Char * replaceEnd = Helper::utf8ReplaceInvalid( _value, _value + _len, replaceMessage, MENGINE_ASSERTION_MAX_MESSAGE );

            Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, "Assertion Validate UTF8", _file, _line, _function, "original string '%s' has invalid utf8"
                , replaceEnd != nullptr ? replaceMessage : "<invalid utf8>"
            );
        }
    }
}
#endif
