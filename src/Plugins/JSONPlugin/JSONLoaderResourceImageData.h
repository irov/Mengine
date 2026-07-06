#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageData.h"

namespace Mengine
{
    class JSONLoaderResourceImageData
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageData );

    public:
        JSONLoaderResourceImageData();
        ~JSONLoaderResourceImageData() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
