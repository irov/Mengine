#pragma once

#include "Interface/PackageServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

#include "Package.h"

namespace Mengine
{
    class PackageService
        : public ServiceBase<PackageServiceInterface>
    {
    public:
        PackageService();
        ~PackageService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadPackages( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;

    public:
        bool hasPackage( const ConstString & _name ) const override;
        bool addPackage( const PackageDesc & _desc, const DocumentInterfacePtr & _doc ) override;
        bool removePackage( const ConstString & _name ) override;
        const PackageInterfacePtr & getPackage( const ConstString & _name ) const override;

    public:
        bool existLocalePackage( const ConstString & _locale, const Tags & _platformTags ) const override;

    public:
        void foreachPackages( const LambdaPackage & _lambda ) const override;

    public:
        void getLocales( const Tags & _platformTags, VectorConstString * const _locales ) const override;

    public:
        bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) override;

    protected:
        bool enableLocalePackages( const ConstString & _locale, const Tags & _platformTag );
        bool disableLocalePackages( const ConstString & _locale, const Tags & _platformTag );

    protected:
        bool disableAllLocalesPackages( const Tags & _platformTag );

    protected:
        void notifyChangeLocale_( const ConstString & _prevLocale, const ConstString & _currentlocale );

    protected:
        typedef Vector<PackageInterfacePtr> VectorPackages;
        VectorPackages m_packages;

        FactoryInterfacePtr m_factoryPackage;

        ConstString m_defaultLocale;

    protected:
        bool loadLocalePacksByName_( const ConstString & _locale, const Tags & _platformTags, VectorPackages * const _packs ) const;
    };
}