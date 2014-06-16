#	pragma once

#	include <stdint.h>

namespace Menge
{
	typedef uint32_t magic_number_type;
	typedef uint32_t magic_version_type;

	template<char c0, char c1, char c2, char c3, uint32_t version>
	struct Magic
	{
		static const magic_number_type magic_number = c0 + (c1 << 8) + (c2 << 16) + (c3 << 24);
		static const magic_version_type magic_version = version;
	};

#	define DECLARE_MAGIC_NUMBER( name, c0, c1, c2, c3, version)\
	typedef Magic<c0, c1, c2, c3, version> name;

#	define GET_MAGIC_NUMBER( name ) (name::magic_number)
#	define GET_MAGIC_VERSION( name ) (name::magic_version)
}