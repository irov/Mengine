#include "Factory.h"

#include "Kernel/Assertion.h"

#include <stdlib.h>

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

            Char msg[1024];
            sprintf( msg, "[Assert] Factory '%s' not empty ['%u']"
                , name
                , count
            );

            Mengine::Assertion( ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Factory Empty" );
        }
    }
}