#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    class JSONLoaderResourceSpineAtlasDefault
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceSpineAtlasDefault );

    public:
        JSONLoaderResourceSpineAtlasDefault();
        ~JSONLoaderResourceSpineAtlasDefault() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
