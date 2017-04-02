#	pragma once

#   include "Interface/TextInterface.h"

#   include "Core/ServiceBase.h"

#	include "TextLocalePak.h"

#	include "TextEntry.h"
#	include "TextFont.h"
#	include "TextGlyph.h"

#   include "Factory/Factory.h"

#	include "Core/ConstString.h"
#   include "Core/ConstStringHolderLocalString.h"

#	include "Core/FilePath.h"

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
		bool loadTextEntry( const ConstString & _pakName, const FilePath & _path ) override;
		bool unloadTextEntry( const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool loadFonts( const ConstString & _pakName, const FilePath & _path ) override;
		bool unloadFonts( const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool existText( const ConstString & _key, const TextEntryInterface ** _entry ) const override;
		const TextEntryInterface * getTextEntry( const ConstString& _key ) const override;
		
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

	public:
		const ConstString & getDefaultFontName() const override;

	public:
		bool addTextEntry( const ConstString& _key
			, const String & _text
			, const ConstString & _font
			, const ColourValue & _colorFont
			, const ColourValue & _colorOutline
			, float _lineOffset
			, float _charOffset
			, float _maxLength
			, ETextHorizontAlign _horizontAlign
			, ETextVerticalAlign _verticalAlign
			, float _scale
			, uint32_t _params
			, bool _isOverride ) override;

		bool removeTextEntry( const ConstString& _key );

	protected:
		TextGlyphPtr loadGlyph_( const ConstString & _pakName, const ConstString & _path );

    protected:
		typedef stdex::map<ConstString, TextEntry> TMapTextEntry;		
		TMapTextEntry m_texts;

		typedef stdex::map<ConstString, TextFontPtr> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::map<ConstString, TextGlyphPtr> TMapTextGlyph;
		TMapTextGlyph m_glyphs;

		typedef stdex::vector<TextLocalePackPtr> TVectorPaks;
		TVectorPaks m_packs;
		
		ConstString m_defaultFontName;

		FactoryPtr m_factoryTextFont;
		FactoryPtr m_factoryTextGlyph;
		FactoryPtr m_factoryTextLocalePak;
		FactoryPtr m_factoryLocalString;

	protected:
        class TextManagerLoadSaxCallback;
        class TextManagerUnloadSaxCallback;

        void createLocalString_( const Char * _text, size_t _size, ConstString & _cstr );
	};
}
