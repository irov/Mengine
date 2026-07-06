#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Factorable.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JSONLoaderInterface
        : public Mixin
    {
    public:
        virtual bool load( const FactorablePtr & _factorable, const jpp::object & _json ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONLoaderInterface> JSONLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
