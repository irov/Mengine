#pragma once

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/ServiceBase.h"

#include "Config/Typedef.h"
#include "Config/Vector.h"
#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#	define MENGINE_RESOURCE_MANAGER_HASH_SIZE 4096
    //////////////////////////////////////////////////////////////////////////
    struct ResourceEntry
    {
        ResourcePtr resource;
        bool isLocked;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ResourcePtr> VectorResources;
    //////////////////////////////////////////////////////////////////////////
    class ResourceManager
        : public ServiceBase<ResourceServiceInterface>
    {
    public:
        ResourceManager();
        ~ResourceManager() override;

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
        bool lockResource( const ConstString& _name );
        bool unlockResource( const ConstString& _name );
        bool hasResourceWithType( const ConstString& _name, const ConstString& _type ) const override;

        PointerResourceReference getResource( const ConstString& _name ) const override;

        PointerResourceReference getResourceReference( const ConstString& _name ) const override; //not compile resource

        const ConstString & getResourceType( const ConstString & _name ) const;

    public:
        void foreachResources( const LambdaResource & _lambda ) const override;

    public:        
        void visitResources( const VisitorPtr & _visitor ) const override;
        void visitGroupResources( const FileGroupInterfacePtr & _category, const ConstString & _group, const VisitorPtr & _visitor ) const override;

    public:
        void dumpResources( const String & _tag ) override;

    protected:
        ResourceEntry * findResource_( const ConstString & _name );
        const ResourceEntry * findResource_( const ConstString & _name ) const;

    protected:
        typedef Map<ConstString, ResourceEntry> MapResources;
        MapResources m_resources[MENGINE_RESOURCE_MANAGER_HASH_SIZE];

        typedef std::pair<ConstString, ConstString> ResourceCacheKey;
        typedef Map<ResourceCacheKey, VectorResources> MapResourceCache;
        MapResourceCache m_resourcesCache;
    };
}
