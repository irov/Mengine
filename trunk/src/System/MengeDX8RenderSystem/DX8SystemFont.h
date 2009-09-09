#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DX8RenderSystem;
	class DX8Texture;

	class DX8SystemFont
	{
	public:
		DX8SystemFont();
		~DX8SystemFont();

		void fontGenerate( const Menge::String& _name, DX8Texture* _texture, int _size, bool _bold, bool _italic, bool _antialias);
		void renderText( DX8RenderSystem* _renderSystem, float _x, float _y, unsigned long _color, const Menge::String& _text);

	private:
		float m_height;
		DX8Texture* m_texture;

		struct CharDesc
		{
			float x;
			float y;
			float w;
			float h;
			float a;
			float c;
			float u0;
			float v0;
			float u1;
			float v1;
		};

		//FIX ME:
		CharDesc	m_letters[256];

		void _fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias, int _leftRange, int _rigthRange);
		bool _placeSymbols( int _width, int _height, int leftRange,  int rigthRange );
	};

}	// namespace Menge
