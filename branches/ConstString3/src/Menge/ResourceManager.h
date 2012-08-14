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
		ConstString name;
		ConstString pakName;
		ConstString pakType;

		WString pakPath;		

		WString path;
		bool script;
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
		bool loadResource( const ResourceDesc & _desc );
		
	public:
		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type );

		template<class T>
		T * createResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
		{
			return static_cast<T*>(this->createResource(_category, _group, _name, _type));
		}

		bool hasResource( const ConstString& _name ) const;
		bool lockResource( const ConstString& _name );
		bool unlockResource( const ConstString& _name );
		bool validResourceType( const ConstString& _name, const ConstString& _type ) const;

		bool validResource( const ConstString& _name ) const;

		ResourceReference * getResource( const ConstString& _name ) const;
		ResourceReference * getResourceReference( const ConstString& _name ) const;

		template<class T>
		T * getResourceT( const ConstString& _name ) const
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

	public:
		void visitResources( const ConstString & _category, const ConstString & _groupName, ResourceVisitor * _visitor ) const;
			

	public:
		bool directResourceCompile( const ConstString& _name );
		void directResourceRelease( const ConstString& _name );

	public:
		void dumpResources( const String & _tag );
		
	protected:
		typedef std::map<ConstString, ResourceEntry> TMapResource;
		TMapResource m_resources;

		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;
	};
}
