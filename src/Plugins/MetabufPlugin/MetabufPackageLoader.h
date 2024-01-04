#pragma once

#include "Interface/PackageInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufPackageLoader
        : public PackageLoaderInterface
        , public Factorable
    {
    public:
        MetabufPackageLoader();
        ~MetabufPackageLoader() override;

    protected:
        bool load( const PackageInterfacePtr & _package, const DocumentInterfacePtr & _doc ) override;
    };
}