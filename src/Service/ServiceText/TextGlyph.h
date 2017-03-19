#	pragma once

#	include "Interface/TextInterface.h"

#	include "TextGlyphChar.h"

#	include "Core/IntrusiveTree.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	class TextGlyph
		: public FactorablePtr
	{
	public:
		TextGlyph();
		~TextGlyph();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _pakName, const FilePath & _path );

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
        bool existGlyphCode( GlyphCode _code ) const;
		const TextGlyphChar * getGlyphChar( GlyphCode _code ) const;               

	public:
		bool addGlyphChar( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size );
        void addKerning( GlyphCode _char, GlyphCode _next, float _kerning );
        float getKerning( GlyphCode _char, GlyphCode _next ) const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_size;
		float m_height;

		float m_ascender;
		float m_descender;

		typedef std::vector<TextGlyphChar> TVectorGlyphChar;
        TVectorGlyphChar m_chars[257];

        struct KerningDesc
        {
            GlyphCode code;
            GlyphCode next;
            float kerning;
        };

        typedef std::vector<KerningDesc> TVectorKerning;
        TVectorKerning m_kernings[257];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextGlyph> TextGlyphPtr;
}