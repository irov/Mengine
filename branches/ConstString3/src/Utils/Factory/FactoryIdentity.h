#	pragma once

#	include "Config/Typedef.h"

#	include <map>
#	include <vector>
#	include <string>

namespace Menge
{
	class FactoryIdentity
	{
	public:
		std::size_t cacheIdentity( const String & _name );

		std::size_t findIdentity( const String & _name ) const;
		const std::string & getIdentity( std::size_t _identity ) const;

	protected:
		typedef std::map<std::string, std::size_t> TMapConstIdentity;
		TMapConstIdentity m_cacheConstIdentity;

		typedef std::vector<TMapConstIdentity::const_iterator> TVectorConstIdentity;
		TVectorConstIdentity m_poolConstIdentity;
	};
}
