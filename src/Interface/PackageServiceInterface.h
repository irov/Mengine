#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/PackageInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class PackageServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PackageService" );

    public:
        virtual bool loadPackages( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool hasPackage( const ConstString & _name ) const = 0;
        virtual bool addPackage( const PackageDesc & _desc, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removePackage( const ConstString & _name ) = 0;
        virtual const PackageInterfacePtr & getPackage( const ConstString & _name ) const = 0;

    public:
        virtual bool existLocalePackage( const ConstString & _locale, const Tags & _platformTags ) const = 0;

    public:
        typedef Lambda<void( const PackageInterfacePtr & )> LambdaPackage;
        virtual void foreachPackages( const LambdaPackage & _lambda ) const = 0;

    public:
        virtual void getLocales( const Tags & _platformTags, VectorConstString * const _locales ) const = 0;

    public:
        virtual bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PACKAGE_SERVICE()\
    ((Mengine::PackageServiceInterface *)SERVICE_GET(Mengine::PackageServiceInterface))
//////////////////////////////////////////////////////////////////////////