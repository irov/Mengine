#include "AssertionFactory.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/MixinDebug.h"
#   include "Kernel/DocumentableHelper.h"

#   include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionFactoryEmpty( const Char * _category, const FactoryInterfacePtr & _factory, const Char * _file, int32_t _line, const Char * _function )
        {
            if( _factory->isEmptyObjects() == true )
            {
                return;
            }

            const ConstString & type = _factory->getType();
            uint32_t count = _factory->getCountObject();

            Char msg[4096 + 1] = {'\0'};

            int32_t msg_offset = 0;
            int32_t msg_size = MENGINE_SNPRINTF( msg + msg_offset, 4096 - msg_offset, "Factory '%s' not empty [%u]\n"
                , type.c_str()
                , count
            );

            msg_offset += msg_size;

#   if defined(MENGINE_DEBUG)
            _factory->foreachFactorables( [&msg, &msg_offset]( const Factorable * _factorable )
            {
                int32_t msg_size = MENGINE_SNPRINTF( msg + msg_offset, 4096 - msg_offset, "Factorable '%s' [%u] doc: %s\n"
                    , MENGINE_MIXIN_DEBUG_NAME( _factorable )
                    , MENGINE_MIXIN_DEBUG_UID( _factorable )
                    , MENGINE_DOCUMENTABLE_STR( _factorable, "None" )
                );

                msg_offset += msg_size;

                if( msg_offset > 3500 )
                {
                    return false;
                }

                return true;
            } );
#   endif

            Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Factory Empty" );
        }
    }
}
#endif
