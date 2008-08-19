#	pragma once

#	include "Config/Typedef.h"

#	include <map>
#	include <string>
#	include <list>

#	include "Holder.h"
#	include "ScriptClassWrapper.h"

class XmlElement;

namespace Menge
{
	class ResourceReference;
	
	class ResourceManagerListener
	{
	public:
		virtual void onResourceLoaded( const std::string& _name ) = 0;
		virtual void onResourceUnLoaded() = 0;
	};

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

	public:
		void loadResource( const std::string & _category, const std::string & _file, const String& _group );

		ResourceReference * createResource( const std::string & _name, const std::string & _type );
		ResourceReference * createResourceFromXml( const std::string& _xml );

		void registerResource( ResourceReference * _resource );
		void unregisterResource( ResourceReference* _resource );
		ResourceReference * getResource( const String& _name );

		template<class T>
		T * getResourceT( const String& _name )
		{
			//return static_cast< T * >( getResource( _name ) );
			T* r = dynamic_cast<T*>( getResource( _name ) );
			//assert( r && "bla" );
			return r;
		}

		void releaseResource( ResourceReference * _resource );

	public:
		bool directResourceCompile( const std::string & _name );
		void directResourceRelease( const std::string & _name );
		void directResourceUnload( const std::string & _name );
		void directResourceFileCompile( const String& _resourceFile );
		void directResourceFileRelease( const String& _resourceFile );
		void directResourceFileUnload( const String& _resourceFile );

		void addListener( ResourceManagerListener* _listener );
		void addListener( PyObject* _listener );
		void removeListener( ResourceManagerListener* _listener );
		void removeListener( PyObject* _listener );

		void loaderDataBlock( XmlElement * _xml );
		void loaderResource( XmlElement * _xml );

#ifdef _DEBUG
		void _dumpResources();
#endif
	protected:

	private:
		typedef std::map< std::string, ResourceReference * > TMapResource;

		String m_currentCategory;
		String m_currentGroup;
		TMapResource m_mapResource;

		typedef std::list< ResourceManagerListener* > TListResourceManagerListener;
		TListResourceManagerListener m_listeners;

		typedef std::map< PyObject*, PyObject* > TMapResourceManagerListenerScript;
		TMapResourceManagerListenerScript m_scriptListeners;


	};
}