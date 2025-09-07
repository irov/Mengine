#pragma once

#include "Interface/PackageInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Kernel/FilePath.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Package
        : public PackageInterface
    {
        DECLARE_FACTORABLE( Package );

    public:
        Package();
        ~Package() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _baseFileGroup, const PackageDesc & _desc ) override;
        void finalize() override;

    public:
        const PackageDesc & getPackageDesc() const override;

    public:
        void setParent( const PackageInterfacePtr & _package ) override;
        const PackageInterfacePtr & getParent() const override;

    public:
        const FileGroupInterfacePtr & getFileGroup() const override;

    public:
        bool load( const DocumentInterfacePtr & _doc ) override;
        bool enable() override;
        bool disable() override;

    public:
        bool isLoad() const override;
        bool isEnable() const override;

    protected:
        void addPackageResource( const FilePath & _filePath, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored ) override;
        void addPackageTextPath( const FilePath & _filePath, const Tags & _platform ) override;
        void addPackageScript( const FilePath & _filePath, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform ) override;
        void addPackageGlyphPath( const FilePath & _filePath, const Tags & _tags ) override;
        void addPackageFontPath( const FilePath & _filePath, const Tags & _tags ) override;
        void addPackageData( const ConstString & _name, const FilePath & _filePath, const Tags & _platform ) override;
        void addPackageMaterial( const FilePath & _filePath, const Tags & _platform ) override;
        void addPackageSetting( const ConstString & _name, const FilePath & _filePath, const Tags & _platform ) override;

    protected:
        bool mountFileGroup_( const DocumentInterfacePtr & _doc );
        void unmountFileGroup_();

    protected:
        bool loadPackage_( const DocumentInterfacePtr & _doc );

    protected:
        struct PackageResourceDesc
        {
            ContentInterfacePtr content;
            Tags tags;
            Tags platform;
            bool demand;
            bool ignored;
        };

        typedef Vector<PackageResourceDesc> VectorPackageResourceDesc;
        VectorPackageResourceDesc m_resourcesDesc;

        PackageInterfacePtr m_parentPackage;

        FileGroupInterfacePtr m_baseFileGroup;

        FileGroupInterfacePtr m_fileGroup;
        PackageDesc m_desc;

        VectorScriptModulePack m_scriptsPackages;

        struct PackageGlyphDesc
        {
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageGlyphDesc> VectorPackageGlyphDesc;
        VectorPackageGlyphDesc m_glyphsDesc;

        struct PackageFontDesc
        {
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageFontDesc> VectorPackageFontDesc;
        VectorPackageFontDesc m_fontsDesc;

        struct PackageTextDesc
        {
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageTextDesc> VectorPackageTextDesc;
        VectorPackageTextDesc m_textsDesc;

        struct PackageDataDesc
        {
            ConstString name;
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageDataDesc> VectorPackageDataDesc;
        VectorPackageDataDesc m_datasDesc;

        struct PackageMaterialDesc
        {
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageMaterialDesc> VectorPackageMaterialDesc;
        VectorPackageMaterialDesc m_materialsDesc;

        struct PackageSettingDesc
        {
            ConstString name;
            ContentInterfacePtr content;
            Tags platform;
        };

        typedef Vector<PackageSettingDesc> VectorPackageSettingDesc;
        VectorPackageSettingDesc m_settingsDesc;

        bool m_load;
        bool m_enable;

    protected:
        bool enableText_( const PackageTextDesc & _desc );
        bool disableText_( const PackageTextDesc & _desc );
        bool enableGlyph_( const PackageGlyphDesc & _desc );
        bool disableGlyph_( const PackageGlyphDesc & _desc );
        bool enableFont_( const PackageFontDesc & _desc );
        bool disableFont_( const PackageFontDesc & _desc );
        bool enableData_( const PackageDataDesc & _desc );
        bool disableData_( const PackageDataDesc & _desc );
        bool enableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc );
        bool disableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc );
        bool enableMaterials_( const PackageMaterialDesc & _desc );
        bool disableMaterials_( const PackageMaterialDesc & _desc );
        bool enableSetting_( const PackageSettingDesc & _desc );
        bool disableSetting_( const PackageSettingDesc & _desc );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Package, PackageInterface> PackagePtr;
    //////////////////////////////////////////////////////////////////////////
}