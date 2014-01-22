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
		bool existText( const ConstString & _key, TextEntryInterfacePtr * _entry ) const override;
		TextEntryInterfacePtr getTextEntry( const ConstString& _key ) const override;
		
	public:
		bool existFont( const ConstString & _name ) const override;

		TextFontInterfacePtr getFont( const ConstString & _name ) override;
		void releaseFont( const TextFontInterfacePtr & _font ) override;
		
	public:
		const ConstString & getDefaultFontName() const override;

	public:
		void addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, size_t _params, bool _isOverride );

	protected:
		TextGlyphPtr loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path );

    protected:
        ServiceProviderInterface * m_serviceProvider;
		
		typedef stdex::binary_vector<ConstString, TextEntryPtr> TMapTextEntry;
		TMapTextEntry m_texts;

		typedef stdex::binary_vector<ConstString, TextFontPtr> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::binary_vector<ConstString, TextGlyphPtr> TMapTextGlyph;
		TMapTextGlyph m_glyphs;

		typedef std::vector<TextLocalePakPtr> TVectorPaks;
		TVectorPaks m_paks;
		
		ConstString m_defaultFontName;

		typedef FactoryPool<TextEntry, 512> TFactoryTextEntry;
		TFactoryTextEntry m_factoryTextEntry;

		typedef FactoryPool<TextFont, 16> TFactoryTextFont;
		TFactoryTextFont m_factoryTextFont;

		typedef FactoryPool<TextGlyph, 16> TFactoryTextGlyph;
		TFactoryTextGlyph m_factoryTextGlyph;

		typedef FactoryPool<TextLocalePak, 4> TFactoryTextLocalePak;
		TFactoryTextLocalePak m_factoryTextLocalePak;
	};
}
