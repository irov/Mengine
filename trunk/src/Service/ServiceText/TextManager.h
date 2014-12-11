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
		bool initialize() override;
		void finalize() override;
		
	public:
		bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;
		bool loadFonts( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool existText( const ConstString & _key, const TextEntryInterface ** _entry ) const override;
		const TextEntryInterface * getTextEntry( const ConstString& _key ) const override;
		
	public:
		bool existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const override;

		TextFontInterfacePtr getFont( const ConstString & _name ) override;
		void releaseFont( const TextFontInterfacePtr & _font ) override;

		void visitFonts( VisitorTextFontInterface * _vistitor ) override;

	public:
		void setEnableText( bool _enable ) override;
		bool getEnableText() const override;

	public:
		bool validate() const override;

	protected:
		void _validateText( TextEntry * _text ) const;
		
	public:
		const ConstString & getDefaultFontName() const override;

	public:
		void addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, uint32_t _params, bool _isOverride );

	protected:
		TextGlyphPtr loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const ConstString & _path );

    protected:
        ServiceProviderInterface * m_serviceProvider;
		
		typedef IntrusiveSplayTree<TextEntry, 256> TMapTextEntry;
		TMapTextEntry m_texts;

		typedef stdex::binary_vector<ConstString, TextFontPtr> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::binary_vector<ConstString, TextGlyphPtr> TMapTextGlyph;
		TMapTextGlyph m_glyphs;

		typedef stdex::vector<TextLocalePakPtr> TVectorPaks;
		TVectorPaks m_paks;
		
		ConstString m_defaultFontName;

		bool m_enableText;

		typedef FactoryPoolStore<TextFont, 16> TFactoryTextFont;
		TFactoryTextFont m_factoryTextFont;

		typedef FactoryPoolStore<TextGlyph, 16> TFactoryTextGlyph;
		TFactoryTextGlyph m_factoryTextGlyph;

		typedef FactoryPoolStore<TextLocalePak, 4> TFactoryTextLocalePak;
		TFactoryTextLocalePak m_factoryTextLocalePak;
	};
}
