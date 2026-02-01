#include "AssertionFactory.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/MixinDebug.h"
#   include "Kernel/DocumentableHelper.h"
#   include "Kernel/Logger.h"

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

            Char msg[1024 + 1] = {'\0'};
            MENGINE_SNPRINTF( msg, 1024, "Factory '%s' not empty [%u]\n"
                , type.c_str()
                , count
            );

#   if defined(MENGINE_DEBUG)
            LOGGER_LOCK_MESSAGES();
            _factory->foreachFactorables( [_category, _file, _line, _function]( const Factorable * _factorable )
            {
                Char factorable_msg[4096 + 1] = {'\0'};
                MENGINE_SNPRINTF( factorable_msg, 4096, "Factorable '%s' [%u] doc: %s\n"
                    , MENGINE_MIXIN_DEBUG_NAME( _factorable )
                    , MENGINE_MIXIN_DEBUG_UID( _factorable )
                    , MENGINE_DOCUMENTABLE_STR( _factorable, "None" )
                );

                Helper::AssertionMessage( _category, _file, _line, _function, factorable_msg );

                return true;
            } );
            LOGGER_UNLOCK_MESSAGES();
#   endif   

            Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Factory Empty" );
        }
    }
}
#endif
