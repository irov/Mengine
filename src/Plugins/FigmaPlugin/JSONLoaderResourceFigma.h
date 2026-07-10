#pragma once

#include "Interface/JSONLoaderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JSONLoaderResourceFigma
        : public JSONLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( JSONLoaderResourceFigma );

    public:
        JSONLoaderResourceFigma();
        ~JSONLoaderResourceFigma() override;

    public:
        bool load( const FactorablePtr & _factorable, const jpp::object & _json ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
