#include "EnumeratorHelper.h"

#include "Interface/EnumeratorServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        UniqueId generateUniqueIdentity()
        {
            UniqueId uniqueIdentity = ENUMERATOR_SERVICE()
                ->generateUniqueIdentity();

            return uniqueIdentity;
        }
    }
}