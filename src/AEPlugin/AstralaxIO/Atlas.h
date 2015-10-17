#	pragma once

#	include "magic.h"

#	include <Windows.h>
#	include <d3d9.h>

#	include <map>

#	include <stdint.h>

namespace Menge
{
	class Atlas
	{
	public:
		Atlas( LPDIRECT3DDEVICE9 pDevice, uint32_t _width, uint32_t _height );

	public:
		bool initialize();
		void finalize();

	public:
		bool loadTexture( const MAGIC_CHANGE_ATLAS & c );
		bool clearTexture( const MAGIC_CHANGE_ATLAS & c );
		
	public:
		void beginRender( int _stage );

	public:
		void release();
		bool restore();

	protected:
		bool createTexure_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 * _texture );
		void renderSprite_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 _texture );
		bool renderTexture_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 _texture );

	protected:
		LPDIRECT3DDEVICE9 m_pDevice;
		uint32_t m_width;
		uint32_t m_height;

		LPDIRECT3DTEXTURE9 m_pTexture;

		struct ImageDesc
		{
			MAGIC_CHANGE_ATLAS c;
			LPDIRECT3DTEXTURE9 texture;
		};

		typedef std::map<uint32_t, ImageDesc> TMapImages;
		TMapImages m_images;
	};
}