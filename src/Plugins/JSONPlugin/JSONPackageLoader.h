#pragma once

#include "Interface/PackageInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class JSONPackageLoader
        : public PackageLoaderInterface
        , public Factorable
    {
    public:
        JSONPackageLoader();
        ~JSONPackageLoader() override;

    protected:
        bool load( const PackageInterfacePtr & _package, const DocumentInterfacePtr & _doc ) override;
    };
}