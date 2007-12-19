#	pragma once

#	include <map>
#	include <string>
#	include <list>

#	include "Holder.h"
#	include "ScriptClassWrapper.h"

namespace Menge
{
	class ResourceReference;

	class ResourceManagerListener
	{
	public:
		virtual void onResourceLoaded() = 0;
		virtual void onResourceUnLoaded() = 0;
	};

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

	public:
		void loadResource( const std::string & _file );

		ResourceReference * createResource( const std::string & _name, const std::string & _type );

		void registerResource( ResourceReference * _resource );
		ResourceReference * getResource( const std::string & _name );

		template<class T>
		T * getResourceT( const std::string & _name )
		{
			return static_cast< T * >( getResource( _name ) );
		}

		void releaseResource( ResourceReference * _resource );

	public:
		bool directResourceCompile( const std::string & _name );
		void directResourceRelease( const std::string & _name );

		void addListener( ResourceManagerListener* _listener );
		void addListener( PyObject* _listener );
		void removeListener( ResourceManagerListener* _listener );
		void removeListener( PyObject* _listener );

	private:
		typedef std::map< std::string, ResourceReference * > TMapResource;

		TMapResource m_mapResource;

		typedef std::list< ResourceManagerListener* > TListResourceManagerListener;
		TListResourceManagerListener m_listeners;

		typedef std::map< PyObject*, PyObject* > TMapResourceManagerListenerScript;
		TMapResourceManagerListenerScript m_scriptListeners;
	};
}