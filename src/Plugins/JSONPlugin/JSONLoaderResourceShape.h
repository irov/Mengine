#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceShape.h"

namespace Mengine
{
    class JSONLoaderResourceShape
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceShape );

    public:
        JSONLoaderResourceShape();
        ~JSONLoaderResourceShape() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
