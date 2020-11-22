#pragma once

#include "Interface/ResourceBankInterface.h"

#include "Kernel/Hashtable.h"
#include "Kernel/Hashtable2.h"

namespace Mengine
{
    class ResourceBank
        : public ResourceBankInterface
    {
    public:
        ResourceBank();
        ~ResourceBank() override;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved ) override;
        void finalize() override;

    public:
        void finalizeKeepResource();

    public:
        ResourcePointer createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _keep, Resource ** const _override, const DocumentPtr & _doc ) override;

    public:
        void removeResource( const ResourcePtr & _resource ) override;

    protected:
        void destroyResource( Resource * _resource ) override;

    public:
        const ResourcePtr & getResource( const ConstString & _groupName, const ConstString & _name ) const override;
        const ResourcePtr & getResourceReference( const ConstString & _groupName, const ConstString & _name ) const override;

        bool hasResource( const ConstString & _groupName, const ConstString & _name, bool _onlyGroup, ResourcePtr * const _resource ) const override;

    protected:
        void foreachResources( const LambdaResourceView & _lambda ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
        
        typedef Hashtable<ConstString, ResourcePtrView> HashtableResources;
        HashtableResources m_resources;

        typedef Hashtable2<ConstString, ConstString, ResourcePtrView> HashtableGroupResources;
        HashtableGroupResources m_resourcesGroup;

        friend class Resource;
        friend class ResourceService;

    protected:
        const HashtableResources & getResources() const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceBank, ResourceBankInterface> ResourceBankPtr;
    //////////////////////////////////////////////////////////////////////////
}