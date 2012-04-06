#	pragma once

#	include "Loadable.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include "ResourceManager.h"

#	include <map>

namespace Menge
{
	struct TextEntry
	{
		WString text;
		ConstString font;
		float charOffset;
		float lineOffset;
	};

	class TextManager
		: public Holder<TextManager>
		, public Loadable
	{
	public:
		TextManager();
		~TextManager();

	public:
		bool loadResource( const ConstString & _name, const ResourceDesc & _desc );

		const TextEntry & getTextEntry( const ConstString& _key ) const;

		bool existText( const ConstString& _key ) const;
		void addTextEntry( const ConstString& _key, const TextEntry& _entry );

		void setDefaultResourceFontName( const ConstString & _fontName );
		const ConstString & getDefaultResourceFontName() const;

	protected:
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

	protected:
		void loaderTexts_( BinParser * _parser );

	protected:
		typedef std::map<ConstString, TextEntry> TMapTextEntry;
		TMapTextEntry m_textMap;

		float m_currentCharOffset;
		ConstString m_currentFont;
		float m_currentLineOffset;

		ConstString m_defaultResourceFontName;
	};
}
