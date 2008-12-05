#	pragma once

#	include "Config/Typedef.h"
#	include "Holder.h"
#	include <map>

class XmlElement;

namespace Menge
{
	class TextManager
	{
	public:
		TextManager();
		virtual ~TextManager();

		bool loadResourceFile( const String& _filename );
		const String& getText( const String& _key ) const;
		void addText( const String& _key, const String& _value );

	protected:
		typedef std::map< String, String > TStringMap;
		TStringMap m_textMap;

	private:
		void loaderResourceFile_( XmlElement* _xml );
	};
}	// namespace Menge