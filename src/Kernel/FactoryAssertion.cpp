#include "Factory.h"

#ifndef NDEBUG
#   include "Kernel/Assertion.h"

#   include <stdlib.h>

namespace Mengine
{
//////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        void assertionFactoryEmpty( const FactoryPtr & _factory )
        {
            if( _factory->isEmptyObjects() == true )
            {
                return;
            }

            const Char * name = _factory->getName();
            uint32_t count = _factory->getCountObject();

            Char msg[1024];
            sprintf( msg, "[Assert] Factory '%s' not empty ['%d']"
                , name
                , count
            );

            Mengine::Assertion( msg, __FILE__, __LINE__ );
        }
    }
}
#endif
