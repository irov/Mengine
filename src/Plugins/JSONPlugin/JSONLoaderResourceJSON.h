#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "ResourceJSON.h"

namespace Mengine
{
    class JSONLoaderResourceJSON
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceJSON );

    public:
        JSONLoaderResourceJSON();
        ~JSONLoaderResourceJSON() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
