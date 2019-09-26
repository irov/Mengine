#pragma once

#include "Interface/PackageInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"

#include "Config/String.h"

namespace Mengine
{
    class Package
        : public PackageInterface
        , public Factorable
    {
    public:
        Package();
        ~Package() override;

    public:
        void initialize( const ConstString & _name
            , const ConstString & _type
            , const ConstString & _locale
            , const Tags & _platform
            , const Tags & _tags
            , const FilePath & _descriptionPath
            , const FileGroupInterfacePtr & _baseFileGroup
            , const FilePath & _filePath
            , bool _preload
        );

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        void setPreload( bool _value ) override;
        bool isPreload() const override;

        void setLocale( const ConstString & _locale ) override;
        const ConstString & getLocale() const override;

        void setPlatfromTags( const Tags & _tags ) override;
        const Tags & getPlatfromTags() const override;

        void setTags( const Tags & _tags ) override;
        const Tags & getTags() const override;

        void setPathPath( const FilePath & _filePath ) override;
        const FilePath & getPathPath() const override;

    public:
        void setParent( const PackageInterfacePtr & _package ) override;
        const PackageInterfacePtr & getParent() const override;

    public:
        const FileGroupInterfacePtr & getFileGroup() const override;

    public:
        bool load( const Char * _doc ) override;
        bool enable() override;
        bool disable() override;

    public:
        bool isLoad() const override;
        bool isEnable() const override;

    protected:
        bool mountFileGroup_( const Char * _doc );
        bool unmountFileGroup_();
        bool loadPackage_();

    protected:
        void addModulePath_( const String & _filePath );

    protected:
        void addResource_( const FilePath & _filePath, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored );
        void addTextPath_( const FilePath & _filePath, const Tags & _platform );
        void addScriptPackage_( const FilePath & _filePath, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform );
        void addFontPath_( const FilePath & _filePath, const Tags & _tags );
        void addData_( const ConstString & _name, const FilePath & _filePath, const Tags & _platform );
        void addMaterial_( const FilePath & _filePath, const Tags & _platform );

    protected:
        bool loadText_( const FilePath & _filePath );
        bool unloadText_( const FilePath & _filePath );
        bool loadFont_( const FilePath & _filePath );
        bool unloadFont_( const FilePath & _filePath );
        bool addUserData_( const ConstString & _name, const FilePath & _filePath );
        bool removeUserData_( const ConstString & _name );
        bool loadMaterials_( const FilePath & _filePath );
        bool unloadMaterials_( const FilePath & _filePath );

    protected:
        struct PackageResourceDesc
        {
            FilePath path;
            Tags tags;
            Tags platform;
            bool demand;
            bool ignored;
        };

        typedef Vector<PackageResourceDesc> VectorPackageResourceDesc;
        VectorPackageResourceDesc m_resourcesDesc;

        ConstString m_name;
        ConstString m_type;
        ConstString m_locale;

        PackageInterfacePtr m_parentPackage;

        FileGroupInterfacePtr m_baseFileGroup;

        Tags m_platform;
        Tags m_tags;
        FilePath m_descriptionPath;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        VectorScriptModulePack m_scriptsPackages;

        struct PackageFontDesc
        {
            FilePath path;
            Tags platform;
        };

        typedef Vector<PackageFontDesc> VectorPackageFontDesc;
        VectorPackageFontDesc m_fontsDesc;

        struct PackageTextDesc
        {
            FilePath path;
            Tags platform;
        };

        typedef Vector<PackageTextDesc> VectorPackageTextDesc;
        VectorPackageTextDesc m_textsDesc;

        struct PackageDataDesc
        {
            ConstString name;
            FilePath path;
            Tags platform;
        };

        typedef Vector<PackageDataDesc> VectorPackageDataDesc;
        VectorPackageDataDesc m_datasDesc;

        struct PackageMaterialDesc
        {
            FilePath path;
            Tags platform;
        };

        typedef Vector<PackageMaterialDesc> VectorPackageMaterialDesc;
        VectorPackageMaterialDesc m_materialsDesc;

        bool m_preload;
        bool m_load;
        bool m_enable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Package, PackageInterface> PackagePtr;
    //////////////////////////////////////////////////////////////////////////
}