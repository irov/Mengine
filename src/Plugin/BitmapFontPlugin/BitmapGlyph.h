#pragma once

#include "Interface/TextInterface.h"

#include "Kernel/Servant.h"

namespace Mengine
{
	struct BitmapGlyphChar
	{
		GlyphCode code;

		mt::vec4f uv;
		mt::vec2f offset;
		float advance;
		mt::vec2f size;
	};

	class BitmapGlyph
		: public Servant
	{
	public:
		BitmapGlyph();
		~BitmapGlyph() override;

	public:
		bool initialize( const FileGroupInterfacePtr & _pakName, const FilePath & _path );

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
		void setTextureWidth( uint32_t _textureWidth );
		void setTextureHeight( uint32_t _textureHeight );
		
	public:
        bool existGlyphCode( GlyphCode _code ) const;
		const BitmapGlyphChar * getGlyphChar( GlyphCode _code ) const;

	public:
		bool addGlyphChar( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size );
        void addKerning( GlyphCode _char, GlyphCode _next, float _kerning );
        float getKerning( GlyphCode _char, GlyphCode _next ) const;

	protected:
		float m_size;
		float m_height;

		float m_ascender;
		float m_descender;

		float m_textureInvWidth;
		float m_textureInvHeight;

		typedef std::vector<BitmapGlyphChar> TVectorGlyphChar;
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
	typedef IntrusivePtr<BitmapGlyph> BitmapGlyphPtr;
}