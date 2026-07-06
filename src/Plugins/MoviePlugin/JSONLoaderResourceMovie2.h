#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    class JSONLoaderResourceMovie2
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceMovie2 );

    public:
        JSONLoaderResourceMovie2();
        ~JSONLoaderResourceMovie2() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
