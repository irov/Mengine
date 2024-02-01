#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pointer.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourcePtrView.h"
#include "Kernel/ResourcePointer.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ResourceCook
    {
        VectorConstString locales;
        ConstString groupName;
        ConstString name;
        ConstString type;
        bool groupCache = false;
        bool keep = false;
    };
    //////////////////////////////////////////////////////////////////////////
    class ResourceBankInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved ) = 0;
        virtual void finalize() = 0;

    public:
        virtual ResourcePointer createResource( const ResourceCook & _cook, Resource ** const _prev, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void removeResource( const ResourcePtr & _resource ) = 0;

    public:
        virtual const ResourcePtr & getResource( const ConstString & _groupName, const ConstString & _name ) const = 0;

    public:
        virtual bool hasResource( const ConstString & _groupName, const ConstString & _name, bool _onlyGroup, ResourcePtr * const _resource ) const = 0;

    public:
        typedef Lambda<void( const ResourcePtrView & )> LambdaResourceView;

        virtual void foreachResources( const LambdaResourceView & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceBankInterface> ResourceBankInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}