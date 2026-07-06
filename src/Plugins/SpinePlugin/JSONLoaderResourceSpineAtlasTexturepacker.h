#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    class JSONLoaderResourceSpineAtlasTexturepacker
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceSpineAtlasTexturepacker );

    public:
        JSONLoaderResourceSpineAtlasTexturepacker();
        ~JSONLoaderResourceSpineAtlasTexturepacker() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
