#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceImageSubstract.h"

namespace Mengine
{
    class JSONLoaderResourceImageSubstract
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceImageSubstract );

    public:
        JSONLoaderResourceImageSubstract();
        ~JSONLoaderResourceImageSubstract() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
