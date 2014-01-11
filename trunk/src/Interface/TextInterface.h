#   pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"
#   include "Core/GlyphChar.h"
#   include "Core/ColourValue.h"

#   include "Factory/FactorablePtr.h"

#	include "Math/vec4.h"

namespace Menge
{
	enum EFontParams
	{
		EFP_NONE			= 0x00000000,
		EFP_FONT			= 0x00000001,
		EFP_COLOR_FONT		= 0x00000002,
		EFP_COLOR_OUTLINE	= 0x00000004,
		EFP_LINE_OFFSET		= 0x00000008,
		EFP_CHAR_OFFSET		= 0x00000010
	};

	struct Glyph
	{
		mt::vec4f uv;
		mt::vec2f offset;
		float advance;
		mt::vec2f size;

		float kerning;
	};

	class TextFontInterface
		: public FactorablePtr
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual bool getGlyph( GlyphCode _char, GlyphCode _prev, Glyph * _glyph ) const = 0;

	public:
		virtual float getFontHeight() const = 0;

	public:
		virtual size_t getFontParams() const = 0;

		virtual const ColourValue & getColorFont() const = 0;
		virtual const ColourValue & getColorOutline() const = 0;

		virtual float getLineOffset() const = 0;
		virtual float getCharOffset() const = 0;

	public:
		virtual const RenderTextureInterfacePtr & getTextureFont() const = 0;
		virtual const RenderTextureInterfacePtr & getTextureOutline() const = 0;
	};

	typedef stdex::intrusive_ptr<TextFontInterface> TextFontInterfacePtr;

	class TextEntryInterface
		: public FactorablePtr
	{
	public:
		virtual const ConstString & getText() const = 0;

	public:
		virtual const ConstString & getFont() const = 0;
		virtual const ColourValue & getColorFont() const = 0;
		virtual const ColourValue & getColorOutline() const = 0;
		virtual float getLineOffset() const = 0;
		virtual float getCharOffset() const = 0;

	public:
		virtual size_t getFontParams() const = 0;
	};

	typedef stdex::intrusive_ptr<TextEntryInterface> TextEntryInterfacePtr;

	class TextServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("TextService")

	public:
		virtual bool initialize( size_t _size ) = 0;

	public:
		virtual bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) = 0;
		virtual bool loadFonts( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual bool existText( const ConstString& _key, TextEntryInterfacePtr * _entry ) const = 0;
		virtual TextEntryInterfacePtr getTextEntry( const ConstString& _key ) const = 0;        

	public:
		virtual bool existFont( const ConstString & _name ) const = 0;

		virtual TextFontInterfacePtr getFont( const ConstString & _name ) = 0;
		virtual void releaseFont( const TextFontInterfacePtr & _font ) = 0;

	public:
		virtual const ConstString & getDefaultFontName() const = 0;
	};

#   define TEXT_SERVICE( serviceProvider )\
	(Menge::Helper::getService<Menge::TextServiceInterface>(serviceProvider))
}