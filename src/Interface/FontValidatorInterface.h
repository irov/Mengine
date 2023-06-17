#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FontInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontValidatorInterface
        : public ServantInterface
    {
    public:
        virtual bool validate( const FontInterfacePtr & _font ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontValidatorInterface> FontValidatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
