#pragma once

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Resource.h"

#include "Kernel/ServiceBase.h"
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
        bool loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path, const Tags & _tags, bool _ignored ) override;
        bool unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path ) override;

    public:
        PointerResourceReference generateResource( const ConstString & _type, const Char * _doc ) const override;

        PointerResourceReference createResource( const ConstString & _locale, const FileGroupInterfacePtr & _category, const ConstString & _group, const ConstString & _name, const ConstString & _type, const Char * _doc ) override;
        bool removeResource( const ResourcePtr & _resource ) override;

    public:
        bool hasResource( const ConstString& _name, ResourcePtr * _resource ) const override;
        bool hasResourceWithType( const ConstString& _name, const ConstString& _type ) const override;

        const ResourcePtr & getResource( const ConstString& _name ) const override;
        const ResourcePtr & getResourceReference( const ConstString& _name ) const override; //not compile resource

        const ConstString & getResourceType( const ConstString & _name ) const;

    public:
        void foreachResources( const LambdaResource & _lambda ) const override;
        void foreachGroupResources( const FileGroupInterfacePtr & _fileGroup, const ConstString & _groupName, const LambdaResource & _lambda ) const override;
        void foreachTagsResources( const Tags & _tags, const LambdaResource & _lambda ) const override;

    public:
        void visitResources( const VisitorPtr & _visitor ) const override;
        void visitGroupResources( const FileGroupInterfacePtr & _fileGroup, const ConstString & _groupName, const VisitorPtr & _visitor ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        typedef Hashtable<ConstString, ResourcePtr> HashtableResources;
        HashtableResources m_resources;

        typedef Pair<ConstString, ConstString> ResourceCacheKey;
        typedef Vector<ResourcePtr> VectorResources;
        typedef Map<ResourceCacheKey, VectorResources> MapResourceCache;
        MapResourceCache m_resourcesCache;
    };
}
