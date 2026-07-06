#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageDefault.h"

namespace Mengine
{
    class JSONLoaderResourceImageDefault
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageDefault );

    public:
        JSONLoaderResourceImageDefault();
        ~JSONLoaderResourceImageDefault() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
