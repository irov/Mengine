#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageEmpty.h"

namespace Mengine
{
    class JSONLoaderResourceImageEmpty
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageEmpty );

    public:
        JSONLoaderResourceImageEmpty();
        ~JSONLoaderResourceImageEmpty() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
