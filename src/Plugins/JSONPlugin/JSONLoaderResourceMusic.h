#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceMusic.h"

namespace Mengine
{
    class JSONLoaderResourceMusic
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceMusic );

    public:
        JSONLoaderResourceMusic();
        ~JSONLoaderResourceMusic() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
