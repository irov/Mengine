#include "BitmapFontValidator.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontValidator::validate( const FactorablePtr & _factorable ) const
    {
        BitmapFontPtr font = BitmapFontPtr::from( _factorable );

        MENGINE_UNUSED( font );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}