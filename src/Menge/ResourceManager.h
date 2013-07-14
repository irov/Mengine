#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Config/Typedef.h"

#   include "stdex/binary_vector.h"

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
	{
	public:
		ResourceManager();
		~ResourceManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool loadResource( const ConstString & _pakName, const ConstString & _path ) override;
		
	public:
		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
        
		bool hasResource( const ConstString& _name, ResourceReference ** _resource ) const override;
		bool lockResource( const ConstString& _name );
		bool unlockResource( const ConstString& _name );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const override;

		bool validResource( const ConstString& _name ) const override;

		ResourceReference * getResource( const ConstString& _name ) const override;
        
        ResourceReference * getResourceReference( const ConstString& _name ) const override; //not compile resource

		const ConstString & getResourceType( const ConstString & _name ) const;

    public:
        void validationResources() const override;

	public:
		void visitResources( ResourceVisitor * _visitor ) const override;
			

	public:
		bool directResourceCompile( const ConstString& _name ) override;
		void directResourceRelease( const ConstString& _name ) override;

	public:
		void dumpResources( const String & _tag );
		
	protected:
        ServiceProviderInterface * m_serviceProvider;
                
		typedef stdex::binary_vector<ConstString, ResourceEntry> TMapResource;
		TMapResource m_resources;
        
		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;
	};
}
