#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "Factory/FactoryManager.h"

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
		virtual void onResourceUnLoaded() = 0;
	};

	class ResourceManager
		: public FactoryManager
		, public Holder<ResourceManager>
	{
	public:
		ResourceManager();
		~ResourceManager();

	public:
		void visitResources( ResourceVisitor * _visitor, const ConstString & _resourceFile );

	public:
		bool loadResource( const ConstString& _category, const ConstString& _group, const ConstString& _file );

		ResourceReference * createResource( const ConstString& _name, const ConstString& _type );

		ResourceReference * createResourceWithParam( const ConstString& _type, const ResourceFactoryParam & _param );
	
		template<class T>
		T * createResourceWithParamT( const ConstString& _type, const ResourceFactoryParam & _param )
		{
			return static_cast<T*>( createResourceWithParam( _type, _param ) );
		}

		ResourceReference * createResourceFromXml( const String& _xml );

		bool registerResource( ResourceReference * _resource );
		void unregisterResource( ResourceReference* _resource );
		ResourceReference * getResource( const ConstString& _name );

		template<class T>
		T * getResourceT( const ConstString& _name )
		{
			T * r = static_cast<T*>( getResource( _name ) );

			return r;
		}

		void releaseResource( ResourceReference * _resource );

		const ConstString& getCategoryResource( const ConstString& _group ) const;

	public:
		bool directResourceCompile( const ConstString& _name );
		void directResourceRelease( const ConstString& _name );
		void directResourceUnload( const ConstString& _name );
		void directResourceFileCompile( const ConstString& _resourceFile );
		void directResourceFileRelease( const ConstString& _resourceFile );
		void directResourceFileUnload( const ConstString& _resourceFile );
		size_t getResourceCount( const ConstString& _resourceFile );

		void addListener( ResourceManagerListener* _listener );
		void addListener( PyObject* _listener );
		void removeListener( ResourceManagerListener* _listener );
		void removeListener( PyObject* _listener );

		void loaderDataBlock( XmlElement * _xml );
		void loaderResource( XmlElement * _xml );

		void dumpResources( const ConstString & _category );
		
	protected:
		ConstString m_currentCategory;
		ConstString m_currentGroup;
		ConstString m_currentFile;

		typedef std::list<ResourceReference *> TListResource;
		typedef std::map<ConstString, TListResource> TCacheGroupResource;
		typedef std::map<ConstString, TListResource::iterator> TMapResource;

		TMapResource m_mapResource;
		TCacheGroupResource m_cacheGroupResource;

		typedef std::list<ResourceManagerListener *> TListResourceManagerListener;
		TListResourceManagerListener m_listeners;

		typedef std::map<PyObject *, PyObject *> TMapResourceManagerListenerScript;
		TMapResourceManagerListenerScript m_scriptListeners;
	
		typedef std::map<ConstString, size_t> TResourceCountMap;
		TResourceCountMap m_resourceCountMap;

		typedef std::map<ConstString, ConstString> TResourcePackMap;
		TResourcePackMap m_resourcePackMap;

	private:
		TListResource::iterator cacheGroupResource_( ResourceReference * _resource );
	};
}
