#pragma once

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "ResourceBank.h"

#include "Kernel/Resource.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/IntrusivePtrView.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"
#include "Kernel/Pair.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceService
        : public ServiceBase<ResourceServiceInterface>
    {
    public:
        ResourceService();
        ~ResourceService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        ResourceBankInterfacePtr createResourceBank( uint32_t _reserved, const DocumentPtr & _doc ) override;

    public:
        ResourcePointer createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _groupCache, bool _keep, const DocumentPtr & _doc ) override;
        void removeResource( const ResourcePtr & _resource ) override;

    public:
        const ResourcePtr & getResourceReference( const ConstString & _groupName, const ConstString & _name ) const override;
        bool hasResource( const ConstString & _groupName, const ConstString & _name, bool _onlyGroup, ResourcePtr * const _resource ) const override;

    public:
        void foreachResources( const LambdaResource & _lambda ) const override;
        void foreachGroupResources( const ConstString & _groupName, const LambdaResource & _lambda ) const override;
        void foreachTagsResources( const Tags & _tags, const LambdaResource & _lambda ) const override;

    public:
        void visitResources( const VisitorPtr & _visitor ) const override;
        void visitGroupResources( const ConstString & _groupName, const VisitorPtr & _visitor ) const override;

    protected:
        void notifyEngineFinalize_();

    protected:
        ThreadMutexInterfacePtr m_mutex;

        ResourceBankPtr m_globalBank;

        typedef IntrusivePtrView<Resource> ResourcePtrView;
        typedef Vector<ResourcePtrView> VectorResources;
        typedef Map<ConstString, VectorResources> MapResourceCache;
        MapResourceCache m_resourcesCache;

        FactoryInterfacePtr m_factoryResourceBank;
    };
}
