#include "AssertionFactory.h"

#if MENGINE_ASSERTION_DEBUG
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        void assertionFactoryEmpty( const FactoryPtr & _factory, const Char * _file, uint32_t _line )
        {
            if( _factory->isEmptyObjects() == true )
            {
                return;
            }

            const Char * name = _factory->getName();
            uint32_t count = _factory->getCountObject();

            Char msg[1024] = {'\0'};
            MENGINE_SNPRINTF( msg, 1024, "[Assert] Factory '%s' not empty ['%u']"
                , name
                , count
            );

            Mengine::Helper::Assertion( ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Factory Empty" );
        }
    }
}
#endif