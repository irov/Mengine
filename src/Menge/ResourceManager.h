#	pragma once

#	include <map>
#	include <string>

#	include "Holder.h"

namespace Menge
{
	class ResourceReference;

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

	private:
		typedef std::map< std::string, ResourceReference * > TMapResource;

		TMapResource m_mapResource;
	};
}