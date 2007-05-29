#	pragma once

#	include <map>
#	include <string>

namespace Menge
{
	class ResourceImageMNG;

	class ResourceImageMNGManager
	{
	public:
		ResourceImageMNGManager();

	public:
		void registerResource( ResourceImageMNG * _resource );
		ResourceImageMNG * getResource( const std::string & _name );

		void releaseResource( ResourceImageMNG * _resource );

	private:
		typedef std::map< std::string, ResourceImageMNG * > TMapResourceSprite;

		TMapResourceSprite m_mapResourceSprite;
	};
}