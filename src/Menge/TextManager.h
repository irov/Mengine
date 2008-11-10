#	pragma once

#	include "Config/Typedef.h"
#	include "Holder.h"
#	include <map>

namespace Menge
{
	class TextManager
	{
	public:
		TextManager();
		virtual ~TextManager();

		bool loadResourceFile( const String& _filename );
		const String& getText( const String& _key );

	protected:
		typedef std::map< String, String > TStringMap;
		TStringMap m_textMap;
	};
}	// namespace Menge