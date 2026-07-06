#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceCursorSystem.h"

namespace Mengine
{
    class JSONLoaderResourceCursorSystem
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceCursorSystem );

    public:
        JSONLoaderResourceCursorSystem();
        ~JSONLoaderResourceCursorSystem() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
