#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceHIT.h"

namespace Mengine
{
    class JSONLoaderResourceHIT
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceHIT );

    public:
        JSONLoaderResourceHIT();
        ~JSONLoaderResourceHIT() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
