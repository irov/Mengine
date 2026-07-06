#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"

namespace Mengine
{
    class JSONLoaderResourceImageSubstractRGBAndAlpha
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageSubstractRGBAndAlpha );

    public:
        JSONLoaderResourceImageSubstractRGBAndAlpha();
        ~JSONLoaderResourceImageSubstractRGBAndAlpha() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
