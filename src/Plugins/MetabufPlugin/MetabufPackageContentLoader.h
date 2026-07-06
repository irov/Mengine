#pragma once

#include "Interface/PackageInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufPackageContentLoader
        : public PackageContentLoaderInterface
        , public Factorable
    {
    public:
        MetabufPackageContentLoader();
        ~MetabufPackageContentLoader() override;

    protected:
        bool enableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc ) override;
        bool disableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc ) override;
        bool enableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc ) override;
        bool disableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc ) override;
    };
}
