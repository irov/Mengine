#include "FactorableHelper.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    namespace Helper
    {
        const ConstString & getFactorableType( const Factorable * _factorable )
        {
            if( _factorable == nullptr )
            {
                return ConstString::none();
            }

            FactoryInterface * factory = _factorable->getFactory();

            if( factory == nullptr )
            {
                return ConstString::none();
            }

            const ConstString & type = factory->getFactoryType();

            return type;
        }
    }
}
