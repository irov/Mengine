#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class XmlElement;

	class TextManager
		: public Holder<TextManager>
	{
	public:
		TextManager();
		virtual ~TextManager();

	public:
		struct TextEntry
		{
			String text;
			ConstString font;
			float charOffset;
			float lineOffset;
		};

	public:
		bool loadResourceFile( const ConstString& _fileSystemName, const String& _filename );
		const TextEntry & getTextEntry( const ConstString& _key ) const;
		bool existText( const ConstString& _key ) const;
		void addTextEntry( const ConstString& _key, const TextEntry& _entry );

	protected:
		typedef std::map<ConstString, TextEntry> TStringMap;
		TStringMap m_textMap;

		float m_currentCharOffset;
		ConstString m_currentFont;
		float m_currentLineOffset;

	private:
		void loaderResourceFile_( XmlElement* _xml );
		void loaderTexts_( XmlElement* _xml );
	};
}
