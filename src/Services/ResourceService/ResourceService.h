#pragma once

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

#include "Config/Typedef.h"
#include "Config/Vector.h"
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
        bool loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pakName, const FilePath & _path, bool _ignored ) override;
        bool unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pakName, const FilePath & _path ) override;

    public:
        PointerResourceReference generateResource( const ConstString& _type ) const override;

        PointerResourceReference createResource( const ConstString & _locale, const FileGroupInterfacePtr& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
        bool removeResource( const ResourcePtr & _resource ) override;

    public:
        bool hasResource( const ConstString& _name, ResourcePtr * _resource ) const override;
        bool hasResourceWithType( const ConstString& _name, const ConstString& _type ) const override;

        const ResourcePtr & getResource( const ConstString& _name ) const override;
        const ResourcePtr & getResourceReference( const ConstString& _name ) const override; //not compile resource

        const ConstString & getResourceType( const ConstString & _name ) const;

    public:
        void foreachResources( const LambdaResource & _lambda ) const override;

    public:        
        void visitResources( const VisitorPtr & _visitor ) const override;
        void visitGroupResources( const FileGroupInterfacePtr & _category, const ConstString & _group, const VisitorPtr & _visitor ) const override;

    protected:
        typedef Hashtable<ConstString, ResourcePtr> HashtableResources;
        HashtableResources m_resources;

        typedef std::pair<ConstString, ConstString> ResourceCacheKey;
        typedef Vector<ResourcePtr> VectorResources;
        typedef Map<ResourceCacheKey, VectorResources> MapResourceCache;
        MapResourceCache m_resourcesCache;
    };
}
