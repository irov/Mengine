#include "AssertionFactory.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionFactoryEmpty( const FactoryPtr & _factory, const Char * _file, uint32_t _line )
        {
            if( _factory->isEmptyObjects() == true )
            {
                return;
            }

            const ConstString & type = _factory->getType();
            uint32_t count = _factory->getCountObject();

            Char msg[1024] = {'\0'};
            MENGINE_SNPRINTF( msg, 1024, "[Assert] Factory '%s' not empty ['%u']"
                , type.c_str()
                , count
            );

            Mengine::Helper::Assertion( ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Factory Empty" );
        }
    }
}
#endif