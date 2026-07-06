#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    class JSONLoaderResourceTexturepacker
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceTexturepacker );

    public:
        JSONLoaderResourceTexturepacker();
        ~JSONLoaderResourceTexturepacker() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
