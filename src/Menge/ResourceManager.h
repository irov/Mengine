#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Factory/FactoryStore.h"

#	include "Config/Typedef.h"

#	include "Core/IntrusiveTree.h"
#	include "Core/IntrusiveDuplexTree.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class ResourceVisitor;
	//////////////////////////////////////////////////////////////////////////
	struct ResourceEntry
		: public Factorable
		, public stdex::intrusive_tree_node<ResourceEntry>
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
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ResourceReference *> TVectorResources;
	//////////////////////////////////////////////////////////////////////////
	struct ResourceCacheEntry
		: public Factorable
		, public stdex::intrusive_duplex_tree_node<ResourceCacheEntry>
	{
		typedef ConstString key_first_type;
		typedef ConstString::less_type less_first_type;

		typedef ConstString key_second_type;
		typedef ConstString::less_type less_second_type;

		struct key_first_getter_type
		{
			const ConstString & operator()( const ResourceCacheEntry * _node ) const
			{
				return _node->category;
			}
		};

		struct key_second_getter_type
		{
			const ConstString & operator()( const ResourceCacheEntry * _node ) const
			{
				return _node->group;
			}
		};

		ConstString category;
		ConstString group;
				
		TVectorResources resources;
	};
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
		bool loadResource( const ConstString & _pakName, const FilePath & _path ) override;
		
	public:
		ResourceReference * generateResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;

		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) override;
		        
	public:
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
		size_t getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group ) const override;

	public:
		void dumpResources( const String & _tag );

	protected:             
		typedef IntrusiveTree<ResourceEntry, 512> TMapResource;
		TMapResource m_resources;

		typedef IntrusiveDuplexTree<ResourceCacheEntry, 32> TMapResourceCache;
		TMapResourceCache m_resourcesCache;
	};
}
