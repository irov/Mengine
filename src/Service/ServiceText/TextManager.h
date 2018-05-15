#pragma once

#include "Interface/TextInterface.h"

#include "Core/ServiceBase.h"

#include "TextLocalePak.h"

#include "TextEntry.h"

#include "Factory/Factory.h"

#include "Core/ConstString.h"
#include "Core/ConstStringHolderLocalString.h"

#include "Core/FilePath.h"

#include "stdex/stl_map.h"
#include "stdex/template_pool.h"

#include "Config/Typedef.h"

namespace Mengine
{
	class TextManager
        : public ServiceBase<TextServiceInterface>
	{
	public:
		TextManager();
		~TextManager() override;

    public:
		bool _initializeService() override;
		void _finalizeService() override;
		
	public:
		bool loadTextEntry( const ConstString & _pakName, const FilePath & _path ) override;
		bool unloadTextEntry( const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool loadFonts( const ConstString & _pakName, const FilePath & _path ) override;
		bool unloadFonts( const ConstString & _pakName, const FilePath & _path ) override;

	public:
		bool existText( const ConstString & _key, TextEntryInterfacePtr * _entry ) const override;
		const TextEntryInterfacePtr & getTextEntry( const ConstString& _key ) const override;
		
	public:
		bool existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const override;
		TextFontInterfacePtr getFont( const ConstString & _name ) const override;

	public:
		void visitFonts( VisitorTextFontInterface * _vistitor ) override;

	public:
		bool validate() const override;

	public:
		const ConstString & getDefaultFontName() const override;

	public:
		bool addTextEntry( const ConstString& _key
			, const String & _text
			, const ConstString & _font
			, const ColourValue & _colorFont
			, float _lineOffset
			, float _charOffset
			, float _maxLength
			, ETextHorizontAlign _horizontAlign
			, ETextVerticalAlign _verticalAlign
			, float _scale
			, uint32_t _params
			, bool _isOverride ) override;

		bool removeTextEntry( const ConstString& _key );

	public:
		bool directFontCompile( const ConstString & _name ) override;
		bool directFontRelease( const ConstString & _name ) override;

    protected:
		typedef stdex::map<ConstString, TextEntryInterfacePtr> TMapTextEntry;
		TMapTextEntry m_texts;

		typedef stdex::map<ConstString, TextFontInterfacePtr> TMapTextFont;
		TMapTextFont m_fonts;

		typedef stdex::vector<TextLocalePackPtr> TVectorPaks;
		TVectorPaks m_packs;
		
		ConstString m_defaultFontName;

        FactoryPtr m_factoryTextEntry;
		FactoryPtr m_factoryTextLocalePak;

        typedef stdex::template_pool<ConstStringHolderLocalString, 1024> PoolConstStringHolderLocalString;
        PoolConstStringHolderLocalString m_poolLocalString;

	protected:
        class TextManagerLoadSaxCallback;
        class TextManagerUnloadSaxCallback;

        void createLocalString_( const Char * _text, size_t _size, ConstString & _cstr );
	};
}
