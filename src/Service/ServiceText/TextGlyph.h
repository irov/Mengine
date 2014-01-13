#	pragma once

#	include "Interface/TextInterface.h"

#	include "TextGlyphChar.h"

#	include "Factory/FactorablePtr.h"
#	include "Factory/FactoryPool.h"

namespace Menge
{
	class TextGlyph
		: public FactorablePtr
	{
	public:
		TextGlyph();
		~TextGlyph();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _locale, const ConstString & _pakName, const FilePath & _path );

	public:
		void setSize( float _size );
		float getSize() const;

		void setHeight( float _size );
		float getHeight() const;

		void setAscender( float _ascender );
		float getAscender() const;

		void setDescender( float _descender );
		float getDescender() const;

	public:
		const TextGlyphChar * getGlyphChar( GlyphCode _code ) const;

	public:
		TextGlyphChar * addGlyphChar( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_locale;

		float m_size;
		float m_height;

		float m_ascender;
		float m_descender;

		typedef stdex::binary_vector<GlyphCode, TextGlyphChar *, GlyphCharLess> TMapGlyphChar;
		TMapGlyphChar m_chars;

		typedef FactoryPool<TextGlyphChar, 256> TFactoryTextGlyphChar;
		TFactoryTextGlyphChar m_factoryTextGlyphChar;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextGlyph> TextGlyphPtr;
}