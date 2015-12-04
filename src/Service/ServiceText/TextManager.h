#	pragma once

#   include "Interface/TextInterface.h"

#	include "TextLocalePak.h"

#	include "TextEntry.h"
#	include "TextFont.h"
#	include "TextGlyph.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class TextManager
        : public ServiceBase<TextServiceInterface>
	{
	public:
		TextManager();
		~TextManager();

    public:
		bool _initialize() override;
		void _finalize() override;
		
	public:
		bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;
		bool loadFonts( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
		void setCurrentLocale( const ConstString & _locale ) override;
		const ConstString & getCurrentLocale() const override;

	public:
		bool existText( const ConstString & _locale, const ConstString & _key, const TextEntryInterface ** _entry ) const override;
		const TextEntryInterface * getTextEntry( const ConstString & _locale, const ConstString& _key ) const override;
		
	public:
		bool existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const override;

		TextFontInterfacePtr getFont( const ConstString & _name ) const override;
		void releaseFont( const TextFontInterfacePtr & _font ) override;

		void visitFonts( VisitorTextFontInterface * _vistitor ) override;

	public:
		bool directFontCompile( const ConstString & _name ) override;
		bool directFontRelease( const ConstString & _name ) override;

	public:
		bool validate() const override;

	protected:
		void _validateText( TextEntry * _text ) const;
		
	public:
		const ConstString & getDefaultFontName() const override;

	public:
		bool addTextEntry( const ConstString & _locale, const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, uint32_t _params, bool _isOverride ) override;

	protected:
		TextGlyphPtr loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const ConstString & _path );

    protected:
		typedef stdex::map<ConstString, TextEntry> TMapTextEntry;
		typedef stdex::map<ConstString, TMapTextEntry> TMapLocaleTextEntry;
		TMapLocaleTextEntry m_texts;

		typedef stdex::map<ConstString, TextFontPtr> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::map<ConstString, TextGlyphPtr> TMapTextGlyph;
		TMapTextGlyph m_glyphs;

		typedef stdex::vector<TextLocalePakPtr> TVectorPaks;
		TVectorPaks m_paks;
		
		ConstString m_currentLocale;
		ConstString m_defaultFontName;

		typedef FactoryPoolStore<TextFont, 16> TFactoryTextFont;
		TFactoryTextFont m_factoryTextFont;

		typedef FactoryPoolStore<TextGlyph, 16> TFactoryTextGlyph;
		TFactoryTextGlyph m_factoryTextGlyph;

		typedef FactoryPoolStore<TextLocalePak, 4> TFactoryTextLocalePak;
		TFactoryTextLocalePak m_factoryTextLocalePak;

	protected:
		const TMapTextEntry * getLocaleTextEntries_( const ConstString & _locale, ConstString & _correctLocale ) const;
	};
}
