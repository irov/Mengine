#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PackageInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PackageServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PackageService" );

    public:
        virtual bool loadPackages( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool hasPackage( const ConstString & _name ) const = 0;
        virtual bool addPackage( const PackageDesc & _desc, const DocumentPtr & _doc ) = 0;
        virtual bool removePackage( const ConstString & _name ) = 0;
        virtual const PackageInterfacePtr & getPackage( const ConstString & _name ) const = 0;

    public:
        virtual bool existLocalePackage( const ConstString & _locale, const Tags & _platformTags ) const = 0;

    public:
        typedef Lambda<void( const PackageInterfacePtr & )> LambdaPackage;
        virtual void visitPackages( const LambdaPackage & _lambda ) const = 0;

    public:
        virtual bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PACKAGE_SERVICE()\
    ((Mengine::PackageServiceInterface*)SERVICE_GET(Mengine::PackageServiceInterface))
//////////////////////////////////////////////////////////////////////////