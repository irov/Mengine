#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/TextFontInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontValidatorInterface
        : public ServantInterface
    {
    public:
        virtual bool validate( const TextFontInterfacePtr & _textFont ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontValidatorInterface> FontValidatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
