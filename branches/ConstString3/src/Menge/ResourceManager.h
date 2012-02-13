#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "FactoryManager.h"

#	include <map>
#	include <list>

namespace Menge
{
	class ResourceReference;
	class ResourceVisitor;

	struct ResourceDesc
	{
		ConstString pak;
		String path;
	};

	struct ResourceEntry
	{
		ResourceReference * resource;
		bool isLocked;
	};

	class ResourceManagerListener
	{
	public:
		virtual void onResourceLoaded( const ConstString& _name ) = 0;
		virtual void onResourceUnLoaded( const ConstString& _name ) = 0;
	};

	class ResourceManager
		: public Holder<ResourceManager>
		, public FactoryManager
	{
	public:
		ResourceManager();
		~ResourceManager();

	public:
		bool loadResource( const ConstString& _name, const ResourceDesc & _desc );
		
	public:
		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type );

		template<class T>
		T * createResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
		{
			return static_cast<T*>(this->createResource(_category, _group, _name, _type));
		}

		bool hasResource( const ConstString& _name ) const;
		void lockResource( const ConstString& _name, bool _lock );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const;

		bool validResource( const ConstString& _name ) const;

		ResourceReference * getResource( const ConstString& _name );
		ResourceReference * getResourceReference( const ConstString& _name );

		template<class T>
		T * getResourceT( const ConstString& _name )
		{
			ResourceReference * resource = this->getResource( _name );

			if( resource == NULL )
			{
				return NULL;
			}

			T * t = dynamic_cast<T*>(resource);

			return t;
		}

		const ConstString & getResourceType( const ConstString & _name ) const;

		bool increfResource( ResourceReference * _resource );

		bool releaseResource( ResourceReference * _resource );

	public:
		bool visitResources( const ConstString& _category, const ConstString& _group, ResourceVisitor * _visitor );

	public:
		bool directResourceCompile( const ConstString& _name );
		void directResourceRelease( const ConstString& _name );

		void directResourceFileCompile( const ConstString& _category, const ConstString& _group );
		void directResourceFileRelease( const ConstString& _category, const ConstString& _group );

		size_t getResourceCount( const ConstString& _category, const ConstString& _group );

		void addListener( ResourceManagerListener* _listener );
		void removeListener( ResourceManagerListener* _listener );

	public:
		void dumpResources( const String & _tag );
		
	protected:
		typedef std::list<ResourceEntry *> TListResource;
		typedef std::map<ConstString, TListResource> TCacheGroupResources;
		typedef std::map<ConstString,TCacheGroupResources> TCacheCategoryResources;
		TCacheCategoryResources m_cacheResources;

		typedef std::map<ConstString, ResourceEntry *> TMapResource;
		TMapResource m_resources;

		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;

	private:
		bool hasGroupResources_( const ConstString& _category, const ConstString& _group ) const;
		TListResource & getGroupResources_( const ConstString& _category, const ConstString& _group );
	};
}
