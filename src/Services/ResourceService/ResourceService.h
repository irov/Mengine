#pragma once

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/IntrusivePtrView.h"
#include "Kernel/Hashtable.h"

#include "Config/Typedef.h"
#include "Config/Vector.h"
#include "Config/Pair.h"
#include "Config/Map.h"

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

    public:
        PointerResourceReference generateResource( const ConstString & _type, const Char * _doc ) const override;

        PointerResourceReference createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _groupCache, bool _global, const Char * _doc ) override;
        bool removeResource( const ResourcePtr & _resource ) override;

    public:
        bool hasResource( const ConstString & _name, ResourcePtr * _resource ) const override;
        bool hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * _resource ) const override;

        const ResourcePtr & getResource( const ConstString & _name ) const override;
        const ResourcePtr & getResourceReference( const ConstString & _name ) const override;

        const ConstString & getResourceType( const ConstString & _name ) const;

    public:
        void foreachResources( const LambdaResource & _lambda ) const override;
        void foreachGroupResources( const ConstString & _groupName, const LambdaResource & _lambda ) const override;
        void foreachTagsResources( const Tags & _tags, const LambdaResource & _lambda ) const override;

    public:
        void visitResources( const VisitorPtr & _visitor ) const override;
        void visitGroupResources( const ConstString & _groupName, const VisitorPtr & _visitor ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        typedef IntrusivePtrView<Resource> ResourcePtrView;
        typedef Hashtable<ConstString, ResourcePtrView> HashtableResources;
        HashtableResources m_resources;
                
        typedef Vector<ResourcePtrView> VectorResources;
        typedef Map<ConstString, VectorResources> MapResourceCache;
        MapResourceCache m_resourcesCache;
    };
}
