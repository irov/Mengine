#include "AssertionVSNPRINTF.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"

#   include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVSNPRINTF( const Char * _category, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args, const Char * _file, int32_t _line, const Char * _function )
        {
            MENGINE_VA_LIST_TYPE args_check;
            MENGINE_VA_LIST_COPY( args_check, _args );

            int32_t format_count = MENGINE_VSNCOUNTF( _format, args_check );

            if( format_count < 0 )
            {
                Char msg[2048 + 1] = {'\0'};
                MENGINE_SNPRINTF( msg, 2048, "VSNPRINTF format '%s' invalid args"
                    , _format
                );

                Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion VSNPRINTF" );

                return;
            }
                        
            if( (size_t)format_count < _capacity )
            {
                return;
            }

            Char msg[2048 + 1] = {'\0'};
            MENGINE_SNPRINTF( msg, 2048, "VSNPRINTF format '%s' capacity [%zu]"
                , _format
                , _capacity
            );

            Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion VSNPRINTF" );
        }
    }
}
#endif
