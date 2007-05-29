#	pragma once

#	include <map>
#	include <string>

namespace Menge
{
	class Resource;

	class ResourceManager
	{
	public:
		ResourceManager();

	public:
		void loadResource( const std::string & _file );

		void registerResource( Resource * _resource );
		Resource * getResource( const std::string & _name );

		template<class T>
		T * getResourceT( const std::string & _name )
		{
			return static_cast< T * >( getResource( _name ) );
		}

		void releaseResource( Resource * _resource );

		typedef std::map< std::string, Resource * > TMapResource;

		TMapResource m_mapResource;
	};
}