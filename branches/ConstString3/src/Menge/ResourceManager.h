#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "FactoryManager.h"

#	include "ScriptClassWrapper.h"

#	include <map>
#	include <list>
#	include "ResourceImageDefault.h"

class XmlElement;

namespace Menge
{
	class ResourceReference;
	class ResourceVisitor;

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
		bool loadResource( const ConstString& _category, const ConstString& _group, const String& _file );

	public:
		ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type );

		template<class T>
		T * createResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
		{
			return static_cast<T*>(this->createResource(_category, _group, _name, _type));
		}

		bool hasResource( const ConstString& _name ) const;
		ResourceReference * getResource( const ConstString& _name );

		template<class T>
		T * getResourceT( const ConstString& _name )
		{
			T * r = static_cast<T*>(this->getResource( _name ) );

			return r;
		}

		bool releaseResource( ResourceReference * _resource );

	public:
		void visitResources( ResourceVisitor * _visitor, const ConstString& _category, const ConstString& _group );

	public:
		bool directResourceCompile( const ConstString& _name );
		void directResourceRelease( const ConstString& _name );

		void directResourceFileCompile( const ConstString& _category, const ConstString& _group );
		void directResourceFileRelease( const ConstString& _category, const ConstString& _group );

		std::size_t getResourceCount( const ConstString& _category, const ConstString& _group );

		void addListener( ResourceManagerListener* _listener );
		void addListener( PyObject* _listener );
		void removeListener( ResourceManagerListener* _listener );
		void removeListener( PyObject* _listener );

	public:
		void dumpResources( const String & _tag );
		
	protected:
		typedef std::list<ResourceReference *> TListResource;
		typedef std::map<ConstString, TListResource> TCacheGroupResources;
		typedef std::map<ConstString,TCacheGroupResources> TCacheCategoryResources;
		TCacheCategoryResources m_cacheResources;

		typedef std::map<ConstString, ResourceReference *> TMapResource;
		TMapResource m_resources;

		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;

	private:
		TListResource & getGroupResources_( const ConstString& _category, const ConstString& _group );
	};
}
