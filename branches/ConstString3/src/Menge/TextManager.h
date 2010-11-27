#	pragma once

#	include "Loadable.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class XmlElement;

	class TextManager
		: public Holder<TextManager>
		, public Loadable
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
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

	protected:
		void loaderTexts_( BinParser * _parser );

	protected:
		typedef std::map<ConstString, TextEntry> TStringMap;
		TStringMap m_textMap;

		float m_currentCharOffset;
		ConstString m_currentFont;
		float m_currentLineOffset;
	};
}
