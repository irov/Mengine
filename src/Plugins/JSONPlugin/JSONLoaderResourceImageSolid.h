#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageSolid.h"

namespace Mengine
{
    class JSONLoaderResourceImageSolid
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageSolid );

    public:
        JSONLoaderResourceImageSolid();
        ~JSONLoaderResourceImageSolid() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
