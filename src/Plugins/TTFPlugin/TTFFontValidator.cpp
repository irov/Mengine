#include "TTFFontValidator.h"

#include "Interface/VocabularyServiceInterface.h"

#include "TTFFont.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontValidator::_validate( const TTFFontPtr & _font ) const
    {
        const FontEffectInterfacePtr & effect = _font->getEffect();

        if( effect != nullptr )
        {
            if( effect->isValid() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}