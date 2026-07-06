#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorConstString.h"

#include "Kernel/PackageDesc.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class PackageInterface> PackageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct PackageResourceDesc
    {
        ContentInterfacePtr content;
        Tags tags;
        Tags platform;
        bool demand;
        bool ignored;
    };
    //////////////////////////////////////////////////////////////////////////
    struct PackageMaterialDesc
    {
        ContentInterfacePtr content;
        Tags platform;
        ConstString renderPlatform;
    };
    //////////////////////////////////////////////////////////////////////////
    class PackageInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const FileGroupInterfacePtr & _baseFileGroup, const PackageDesc & _desc ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const PackageDesc & getPackageDesc() const = 0;

    public:
        virtual void setParent( const PackageInterfacePtr & _package ) = 0;
        virtual const PackageInterfacePtr & getParent() const = 0;

    public:
        virtual const FileGroupInterfacePtr & getFileGroup() const = 0;

    public:
        virtual bool load( const DocumentInterfacePtr & _doc ) = 0;
        virtual bool enable() = 0;
        virtual bool disable() = 0;

    public:
        virtual bool isLoad() const = 0;
        virtual bool isEnable() const = 0;

    public:
        virtual void addPackageResource( const FilePath & _filePath, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored ) = 0;
        virtual void addPackageTextPath( const FilePath & _filePath, const Tags & _platform ) = 0;
        virtual void addPackageScript( const FilePath & _filePath, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform ) = 0;
        virtual void addPackageGlyphPath( const FilePath & _filePath, const Tags & _tags ) = 0;
        virtual void addPackageFontPath( const FilePath & _filePath, const Tags & _tags ) = 0;
        virtual void addPackageData( const ConstString & _name, const FilePath & _filePath, const Tags & _platform ) = 0;
        virtual void addPackageMaterial( const FilePath & _filePath, const Tags & _platform ) = 0;
        virtual void addPackageMaterial( const FilePath & _filePath, const Tags & _platform, const ConstString & _renderPlatform ) = 0;
        virtual void addPackageSetting( const ConstString & _name, const FilePath & _filePath, const Tags & _platform ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PackageInterface> PackageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PackageLoaderInterface
        : public Mixin
    {
    public:
        virtual bool load( const PackageInterfacePtr & _package, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PackageLoaderInterface> PackageLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PackageContentLoaderInterface
        : public Mixin
    {
    public:
        virtual bool enableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc ) = 0;
        virtual bool disableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc ) = 0;
        virtual bool enableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc ) = 0;
        virtual bool disableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PackageContentLoaderInterface> PackageContentLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
