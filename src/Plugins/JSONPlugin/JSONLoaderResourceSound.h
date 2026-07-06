#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Kernel/ResourceSound.h"

namespace Mengine
{
    class JSONLoaderResourceSound
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceSound );

    public:
        JSONLoaderResourceSound();
        ~JSONLoaderResourceSound() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
