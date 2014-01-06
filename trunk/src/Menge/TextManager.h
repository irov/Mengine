#	pragma once

#   include "Interface/TextInterface.h"

#	include "TextLocalePak.h"

#	include "TextEntry.h"
#	include "TextFont.h"
#	include "TextGlyph.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class TextManager
        : public TextServiceInterface
	{
	public:
		TextManager();
		~TextManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
		bool initialize( size_t _size ) override;
		
	public:
		bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;
		bool loadFonts( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool existText( const ConstString & _key, const TextEntryInterface ** _entry ) const override;
		const TextEntryInterface * getTextEntry( const ConstString& _key ) const override;
		
	public:
		bool existFont( const ConstString & _name ) const override;

		TextFontInterface * getFont( const ConstString & _name ) override;
		void releaseFont( TextFontInterface * _font ) override;
		
	public:
		void setDefaultResourceFontName( const ConstString & _fontName ) override;
		const ConstString & getDefaultResourceFontName() const override;

	public:
		void addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, size_t _params );

	protected:
		const TextGlyph * loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path );

    protected:
        ServiceProviderInterface * m_serviceProvider;
		
		typedef stdex::binary_vector<ConstString, TextEntry *> TMapTextEntry;
		TMapTextEntry m_texts;

		typedef stdex::binary_vector<ConstString, TextFont *> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::binary_vector<ConstString, TextGlyph *> TMapTextGlyph;
		TMapTextGlyph m_glyphs;

		typedef std::vector<TextLocalePak *> TVectorPaks;
		TVectorPaks m_paks;
		
		ConstString m_defaultResourceFontName;

		typedef FactoryPool<TextEntry, 512> TFactoryTextEntry;
		TFactoryTextEntry m_factoryTextEntry;

		typedef FactoryPool<TextFont, 16> TFactoryTextFont;
		TFactoryTextFont m_factoryTextFont;

		typedef FactoryPool<TextGlyph, 16> TFactoryTextGlyph;
		TFactoryTextGlyph m_factoryTextGlyph;
	};
}
