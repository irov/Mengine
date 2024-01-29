#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FactorablePointer generatePrototype( const ConstString & _category, const ConstString & _prototype, const DocumentInterfacePtr & _doc )
        {
            FactorablePointer prototype = PROTOTYPE_SERVICE()
                ->generatePrototype( _category, _prototype, _doc );

            return prototype;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}