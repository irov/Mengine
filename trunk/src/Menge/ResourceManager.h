#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Config/Typedef.h"

#	include "Factory/FactoryManager.h"

#	include <map>
#	include <list>

namespace Menge
{
	class ResourceReference;
	class ResourceVisitor;

	struct ResourceEntry
	{
		ResourceReference * resource;
		bool isLocked;
	};

	class ResourceManager
		: public ResourceServiceInterface
        , public FactoryManager
	{
	public:
		ResourceManager();
		~ResourceManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        void registerResourceFactory( const ConstString & _type, Factory * _factory ) override;

	public:
		bool loadResource( const ResourceDesc & _desc ) override;
		
	public:
		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
        
		bool hasResource( const ConstString& _name ) const override;
		bool lockResource( const ConstString& _name );
		bool unlockResource( const ConstString& _name );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const override;

		bool validResource( const ConstString& _name ) const override;

		ResourceReference * getResource( const ConstString& _name ) const override;
        
        ResourceReference * getResourceReference( const ConstString& _name ) const override; //not compile resource

		const ConstString & getResourceType( const ConstString & _name ) const;

	public:
		void visitResources( const ConstString & _category, const ConstString & _groupName, ResourceVisitor * _visitor ) const override;
			

	public:
		bool directResourceCompile( const ConstString& _name ) override;
		void directResourceRelease( const ConstString& _name ) override;

	public:
		void dumpResources( const String & _tag );
		
	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, ResourceEntry> TMapResource;
		TMapResource m_resources;

        typedef std::vector<ResourceReference *> TVectorResource;
        typedef std::map<ConstString, TVectorResource> TMapGroupCacheResource;
        typedef std::map<ConstString, TMapGroupCacheResource> TMapCategoryCacheResource;

        TMapCategoryCacheResource m_cacheResource;

		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;
	};
}
