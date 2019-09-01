#pragma once

#include "Interface/Interface.h"

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

        ConstString category;
        FilePath descriptionPath;
        FilePath path;

        bool dev;
        bool immediately;
        bool preload;
    };
    //////////////////////////////////////////////////////////////////////////
    class PackageInterface
        : public Interface
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

        virtual void setPathPath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getPathPath() const = 0;

    public:
        virtual bool load( const Char * _doc ) = 0;
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