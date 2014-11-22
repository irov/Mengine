#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Factory/FactoryStore.h"

#	include "Config/Typedef.h"
#	include "Core/IntrusiveSprayTree.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class ResourceVisitor;

	struct ResourceEntry
		: public Factorable
		, public stdex::intrusive_splay_node<ResourceEntry>
	{
		typedef ConstString key_type;
		typedef ConstString::less_type less_type;

		struct key_getter_type
		{
			const ConstString & operator()( const ResourceEntry * _node ) const
			{
				return _node->resource->getName();
			}
		};

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
		bool initialize() override;
		void finalize() override;

	public:
		bool loadResource( const ConstString & _pakName, const FilePath & _path ) override;
		
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
        bool validationResources() const override;

	public:
		void visitResources( ResourceVisitor * _visitor ) const override;
		void visitGroupResources( const ConstString & _category, const ConstString & _group, ResourceVisitor * _visitor ) const override;
			
	public:
		void dumpResources( const String & _tag );

	protected:
        ServiceProviderInterface * m_serviceProvider;
               
		typedef IntrusiveSprayTree<ResourceEntry, 512> TMapResource;
		TMapResource m_resources;

		typedef stdex::vector<ResourceReference *> TVectorResources;
		typedef stdex::binary_vector<ConstString, TVectorResources> TMapGroupResourceCache;
		typedef stdex::binary_vector<ConstString, TMapGroupResourceCache> TMapCategoryResourceCache;
		TMapCategoryResourceCache m_resourcesCache;
	};
}
