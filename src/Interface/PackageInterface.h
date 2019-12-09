#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct PackageDesc
    {
        ConstString name;
        ConstString type;

        ConstString locale;
        Tags platform;
        Tags tags;

        ConstString fileGroupName;
        ConstString parent;
        FilePath descriptionPath;
        FilePath path;

        FilePath fontsPath;
        FilePath textsPath;

        bool dev;
        bool immediately;
        bool preload;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class PackageInterface> PackageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PackageInterface
        : public ServantInterface
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual void setPreload( bool _value ) = 0;
        virtual bool isPreload() const = 0;

        virtual void setLocale( const ConstString & _locale ) = 0;
        virtual const ConstString & getLocale() const = 0;

        virtual void setPlatfromTags( const Tags & _platform ) = 0;
        virtual const Tags & getPlatfromTags() const = 0;

        virtual void setTags( const Tags & _tags ) = 0;
        virtual const Tags & getTags() const = 0;

        virtual void setPathPath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getPathPath() const = 0;

    public:
        virtual void setParent( const PackageInterfacePtr & _package ) = 0;
        virtual const PackageInterfacePtr & getParent() const = 0;

    public:
        virtual const FileGroupInterfacePtr & getFileGroup() const = 0;

    public:
        virtual bool load( const DocumentPtr & _doc ) = 0;
        virtual bool enable() = 0;
        virtual bool disable() = 0;

    public:
        virtual bool isLoad() const = 0;
        virtual bool isEnable() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PackageInterface> PackageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}