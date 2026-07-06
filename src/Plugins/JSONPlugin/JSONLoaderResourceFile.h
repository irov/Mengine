#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceFile.h"

namespace Mengine
{
    class JSONLoaderResourceFile
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceFile );

    public:
        JSONLoaderResourceFile();
        ~JSONLoaderResourceFile() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
