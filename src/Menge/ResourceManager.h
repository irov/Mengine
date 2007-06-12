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
		~ResourceManager();

	public:
		void loadResource( const std::string & _file );

		Resource * createResource( const std::string & _name, const std::string & _type );

		void registerResource( Resource * _resource );
		Resource * getResource( const std::string & _name );

		template<class T>
		T * getResourceT( const std::string & _name )
		{
			return static_cast< T * >( getResource( _name ) );
		}

		void releaseResource( Resource * _resource );

	private:
		typedef std::map< std::string, Resource * > TMapResource;

		TMapResource m_mapResource;
	};
}