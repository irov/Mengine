#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Factory/FactoryStore.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_map.h>

namespace Menge
{
	class ResourceVisitor;
	//////////////////////////////////////////////////////////////////////////
#	define MENGINE_RESOURCE_MANAGER_HASH_SIZE 4096
	//////////////////////////////////////////////////////////////////////////
	struct ResourceEntry
	{
		ResourceReferencePtr resource;
		bool isLocked;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ResourceReferencePtr> TVectorResources;
	//////////////////////////////////////////////////////////////////////////
	class ResourceManager
		: public ServiceBase<ResourceServiceInterface>
	{
	public:
		ResourceManager();
		~ResourceManager();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path, bool _ignored ) override;
		bool unloadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
		ResourceReferencePtr generateResource( const ConstString& _type ) const override;

		ResourceReferencePtr createResource( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
		bool removeResource( const ResourceReferencePtr & _resource ) override;

	public:
		bool hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const override;
		bool lockResource( const ConstString& _name );
		bool unlockResource( const ConstString& _name );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const override;

		bool validResource( const ConstString& _name ) const override;

		ResourceReferencePtr getResource( const ConstString& _name ) const override;
        
        ResourceReferencePtr getResourceReference( const ConstString& _name ) const override; //not compile resource

		const ConstString & getResourceType( const ConstString & _name ) const;

    public:
		bool validateResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) const override;

	public:
		void visitResources( Visitor * _visitor ) const override;
		void visitGroupResources( const ConstString & _category, const ConstString & _group, Visitor * _visitor ) const override;
			
	public:
		size_t getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group ) const override;

	public:
		void dumpResources( const String & _tag );

	protected:
		ResourceEntry * findResource_( const ConstString & _name );
		const ResourceEntry * findResource_( const ConstString & _name ) const;

	protected:
		typedef stdex::map<ConstString, ResourceEntry> TMapResource;
		TMapResource m_resources[MENGINE_RESOURCE_MANAGER_HASH_SIZE];

		typedef std::pair<ConstString, ConstString> TResourceCacheKey;
		typedef stdex::map<TResourceCacheKey, TVectorResources> TMapResourceCache;
		TMapResourceCache m_resourcesCache;
	};
}
