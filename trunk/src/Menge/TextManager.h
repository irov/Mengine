#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"
#	include <map>

class XmlElement;

namespace Menge
{
	class TextManager
		: public Holder<TextManager>
	{
	public:

		struct TextEntry
		{
			String text;
			String font;
			float charOffset;
			float lineOffset;
		};

		TextManager();
		virtual ~TextManager();

		bool loadResourceFile( const String& _fileSystemName, const String& _filename );
		TextEntry getTextEntry( const String& _key ) const;
		void addTextEntry( const String& _key, const TextEntry& _entry );

	protected:
		typedef std::map< String, TextEntry > TStringMap;
		TStringMap m_textMap;

	private:
		void loaderResourceFile_( XmlElement* _xml );
		void loaderTexts_( XmlElement* _xml );

		float m_currentCharOffset;
		String m_currentFont;
		float m_currentLineOffset;
	};
}
