#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceCursorICO.h"

namespace Mengine
{
    class JSONLoaderResourceCursorICO
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceCursorICO );

    public:
        JSONLoaderResourceCursorICO();
        ~JSONLoaderResourceCursorICO() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
