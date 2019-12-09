#pragma once

#include "Interface/PackageServiceInterface.h"

#include "Kernel/Observable.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "Package.h"

namespace Mengine
{
    class PackageService
        : public ServiceBase<PackageServiceInterface>
        , public Observable
    {
    public:
        PackageService();
        ~PackageService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadPackages( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) override;

    public:
        bool hasPackage( const ConstString & _name ) const override;
        bool addPackage( const PackageDesc & _desc, const DocumentPtr & _doc ) override;
        bool removePackage( const ConstString & _name ) override;
        const PackageInterfacePtr & getPackage( const ConstString & _name ) const override;

    public:        
        void visitPackages( const LambdaPackage & _lambda ) const override;

    public:
        bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) override;

    public:
        bool enableLocalePackage( const ConstString & _locale, const Tags & _platformTag );
        bool disableLocalePackage( const ConstString & _locale, const Tags & _platformTag );

    protected:
        void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );

    protected:
        typedef Vector<PackagePtr> VectorPackages;
        VectorPackages m_packages;

        FactoryPtr m_factoryPackage;

    protected:
        bool loadLocalePacksByName_( VectorPackages & _packs, const ConstString & _locale, const Tags & _platformTags ) const;
    };
}