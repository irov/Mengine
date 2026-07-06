#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageSequence.h"

namespace Mengine
{
    class JSONLoaderResourceImageSequence
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageSequence );

    public:
        JSONLoaderResourceImageSequence();
        ~JSONLoaderResourceImageSequence() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
