#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    class JSONLoaderResourceSpineSkeleton
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceSpineSkeleton );

    public:
        JSONLoaderResourceSpineSkeleton();
        ~JSONLoaderResourceSpineSkeleton() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
