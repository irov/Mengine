#pragma once

#include "Interface/JSONLoaderInterface.h"

#include "Engine/ResourceWindow.h"

namespace Mengine
{
    class JSONLoaderResourceWindow
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceWindow );

    public:
        JSONLoaderResourceWindow();
        ~JSONLoaderResourceWindow() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
}
