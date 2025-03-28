#include "AssertionUtf8.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/Utf8Helper.h"

#   include "Config/StdIO.h"
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

            if( Helper::Utf8Validate( _value, _value + _len, nullptr ) == false )
            {
                Char replace_msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                const Char * replace_msg_end = Helper::Utf8ReplaceInvalid( _value, _value + _len, replace_msg);
                size_t replace_msg_len = replace_msg_end - replace_msg;

                Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "original string '%.*s' has invalid utf8"
                    , (int32_t)replace_msg_len
                    , replace_msg_end
                );

                Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Validate UTF8" );
            }
        }
    }
}
#endif
