#pragma once

#include "Interface/ResourceInterface.h"

#include "Kernel/ResourceReference.h"

#include "Core/ServiceBase.h"

#include "Config/Typedef.h"

#include "stdex/stl_map.h"

namespace Mengine
{
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
		~ResourceManager() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path, bool _ignored ) override;
		bool unloadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
        PointerResourceReference generateResource( const ConstString& _type ) const override;

        PointerResourceReference createResource( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
		bool removeResource( const ResourceReferencePtr & _resource ) override;

	public:
		bool hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const override;
		bool lockResource( const ConstString& _name );
		bool unlockResource( const ConstString& _name );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const override;

		bool validResource( const ConstString& _name ) const override;

        PointerResourceReference getResource( const ConstString& _name ) const override;
        
        PointerResourceReference getResourceReference( const ConstString& _name ) const override; //not compile resource

		const ConstString & getResourceType( const ConstString & _name ) const;

    public:
		bool validateResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) const override;

	public:
		void visitResources( Visitor * _visitor ) const override;
		void visitGroupResources( const ConstString & _category, const ConstString & _group, Visitor * _visitor ) const override;
			
	public:
		void dumpResources( const String & _tag ) override;

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
