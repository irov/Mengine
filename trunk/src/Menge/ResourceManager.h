#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Config/Typedef.h"

#   include "stdex/binary_vector.h"
#   include "stdex/binary_set.h"

#	include <map>
#	include <list>

namespace Menge
{
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
		bool initialize( size_t _reserved ) override;

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
		void visitGroupResources( const ConstString & _category, const ConstString & _group, ResourceVisitor * _visitor ) const override;
			

	public:
		bool directResourceCompile( const ConstString& _name ) override;
		void directResourceRelease( const ConstString& _name ) override;

	public:
		void dumpResources( const String & _tag );
		
	protected:
        ServiceProviderInterface * m_serviceProvider;

		struct ResourceEntryGetKey
		{
			const ConstString & operator()( const ResourceEntry & _entry )
			{
				const ConstString & name = _entry.resource->getName();

				return name;
			}
		};
                
		typedef stdex::binary_set<ConstString, ResourceEntry, ResourceEntryGetKey> TMapResource;
		TMapResource m_resources;

		struct CategoryGroupKey
		{
			ConstString category;
			ConstString group;
		};

		struct CategoryGroupLess
		{
			bool operator () ( const CategoryGroupKey & _left, const CategoryGroupKey & _right ) const
			{
				if( _left.category < _right.category )
				{
					return true;
				}
				else if( _left.category == _right.category )
				{
					return _left.group < _right.group;
				}

				return false;
			}
		};

		typedef std::vector<ResourceReference *> TVectorResources;
		typedef stdex::binary_vector<CategoryGroupKey, TVectorResources, CategoryGroupLess> TMapResourceCache;
		TMapResourceCache m_resourcesCache;
        
		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;
	};
}
