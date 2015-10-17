#	include "Atlas.h"

#	include "Image.h"

#	include "Math/mat4.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define DXCALL
	//////////////////////////////////////////////////////////////////////////
	struct VERTEX_SPRITE
	{
		float x, y, z;
		uint32_t color;
		float u, v;
	};
	//////////////////////////////////////////////////////////////////////////
	static const DWORD FVF_SPRITE = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t getTexturePOW2( uint32_t n )
	{
		--n;
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		++n;

		return n;
	}
	//////////////////////////////////////////////////////////////////////////
	Atlas::Atlas( LPDIRECT3DDEVICE9 _pDevice, uint32_t _width, uint32_t _height )
		: m_pDevice(_pDevice)
		, m_width(_width)
		, m_height(_height)
		, m_pTexture(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::initialize()
	{		
		if( m_pDevice->CreateTexture( m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL ) != D3D_OK )
		{
			//_logger.message("Atlas::initialize create texture %d:%d"
			//	, m_width
			//	, m_height
			//	);

			return false;
		}

		IDirect3DSurface9 * back = NULL;
		DXCALL m_pDevice->GetRenderTarget( 0, &back );

		if( back == NULL )
		{
			//_logger.message("Atlas::initialize back == NULL"
			//	);

			return false;
		}

		IDirect3DSurface9 * dest = NULL;
		m_pTexture->GetSurfaceLevel( 0, &dest );

		if( dest == NULL )
		{
			//_logger.message("Atlas::initialize dest == NULL"
			//	);

			return false;
		}

		DXCALL m_pDevice->SetRenderTarget( 0, dest );

		m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x0, 0.0f, 0 );
		
		dest->Release();

		m_pDevice->SetRenderTarget( 0, back );

		back->Release();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Atlas::finalize()
	{
		this->release();

		for( TMapImages::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			ImageDesc & desc = it->second;

			desc.texture->Release();
		}

		m_images.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Atlas::release()
	{
		if( m_pTexture != NULL )
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::restore()
	{
		for( TMapImages::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			ImageDesc & desc = it->second;

			if( this->renderTexture_( desc.c, desc.texture ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::createTexure_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 * _texture )
	{
		uint32_t image_width;
		uint32_t image_height;

		unsigned char * pixels = nullptr;

		if( c.data == nullptr )
		{
			pixels = imageLoadFromFile( c.file, image_width, image_height );
		}
		else
		{
			pixels = imageLoadFromMemory( c.data, c.length, image_width, image_height );
		}

		unsigned char * correct_pixels = pixels;

		UINT texture_width = getTexturePOW2( image_width );
		UINT texture_height = getTexturePOW2( image_height );

		LPDIRECT3DTEXTURE9 texture = NULL;
		if( m_pDevice->CreateTexture( texture_width, texture_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL ) != D3D_OK )
		{
			return false;
		}

		D3DLOCKED_RECT lockedRect;
		DXCALL texture->LockRect( 0, &lockedRect, NULL, D3DLOCK_DISCARD );

		if( lockedRect.pBits == NULL || lockedRect.Pitch == 0 )
		{
			return false;
		}

		for( size_t i = 0; i != image_height; ++i )
		{
			unsigned char * image = correct_pixels + i * image_width * 4;
			unsigned char * bits = static_cast<unsigned char *>(lockedRect.pBits) + lockedRect.Pitch * i;

			std::copy( image, image + image_width * 4, bits );
		}

		DXCALL texture->UnlockRect( 0 );

		imageFree( pixels );

		*_texture = texture;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_imageIndex( const MAGIC_CHANGE_ATLAS & c )
	{
		uint32_t index = c.x + (c.y * c.width);

		return index;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::loadTexture( const MAGIC_CHANGE_ATLAS & c )
	{
		uint32_t index = s_imageIndex( c );

		TMapImages::iterator it_found = m_images.find( index );

		if( it_found != m_images.end() )
		{
			//_logger.message("Atlas::loadTexture already load!"
			//	);

			return false;
		}

		LPDIRECT3DTEXTURE9 texture_frame = NULL;
		if( this->createTexure_( c, &texture_frame ) == false )
		{
			return false;
		}

		ImageDesc desc;

		desc.c = c;
		desc.texture = texture_frame;

		m_images[index] = desc;

		if( this->renderTexture_( c, texture_frame ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Atlas::renderSprite_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 _texture )
	{
		D3DSURFACE_DESC texDesc;
		DXCALL _texture->GetLevelDesc( 0, &texDesc );

		float cw = (float)c.width;
		float ch = (float)c.height;

		float tw = (float)texDesc.Width;
		float th = (float)texDesc.Height;

		float uv_x = cw / tw;
		float uv_y = ch / th;

		VERTEX_SPRITE v[4];

		v[0].x = (float)c.x;
		v[0].y = (float)c.y;
		v[0].z = 0.f;
		v[0].color = 0xffffffff;
		v[0].u = 0.f;
		v[0].v = 0.f;

		v[1].x = (float)c.x + (float)c.width;
		v[1].y = (float)c.y;
		v[1].z = 0.f;
		v[1].color = 0xffffffff;
		v[1].u = uv_x;
		v[1].v = 0.f;

		v[2].x = (float)c.x;
		v[2].y = (float)c.y + (float)c.height;
		v[2].z = 0.f;
		v[2].color = 0xffffffff;
		v[2].u = 0.f;
		v[2].v = uv_y;

		v[3].x = (float)c.x + (float)c.width;
		v[3].y = (float)c.y + (float)c.height;
		v[3].z = 0.f;
		v[3].color = 0xffffffff;
		v[3].u = uv_x;
		v[3].v = uv_y;

		D3DVIEWPORT9 VP;
		VP.X = 0;
		VP.Y = 0;
		VP.Width = m_width;
		VP.Height = m_height;
		VP.MinZ = 0;
		VP.MaxZ = 1.f;

		DXCALL m_pDevice->SetViewport( &VP );

		mt::mat4f ortho;
		mt::make_projection_ortho_lh_m4( ortho, 0, (float)m_width, (float)m_height, 0, -1000.f, 1000.f );

		DXCALL m_pDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)ortho.buff() );

		mt::mat4f view;
		mt::ident_m4( view );

		DXCALL m_pDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)view.buff() );

		DXCALL m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		for( int stage = 0; stage != 8; ++stage )
		{
			DXCALL m_pDevice->SetTexture( stage, nullptr );
			DXCALL m_pDevice->SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
			DXCALL m_pDevice->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}

		DXCALL m_pDevice->SetTexture( 0, _texture );

		DXCALL m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		DXCALL m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		DXCALL m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		DXCALL m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		DXCALL m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );

		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		DXCALL m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

		DXCALL m_pDevice->SetFVF( FVF_SPRITE );
		DXCALL m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX_SPRITE) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::renderTexture_( const MAGIC_CHANGE_ATLAS & c, LPDIRECT3DTEXTURE9 _texture )
	{
		IDirect3DSurface9 * dest = NULL;
		DXCALL m_pTexture->GetSurfaceLevel( 0, &dest );

		if( dest == NULL )
		{
			//_logger.message("Atlas::loadTexture dest == NULL"
			//	);

			return false;
		}

		IDirect3DSurface9 * back = NULL;
		DXCALL m_pDevice->GetRenderTarget( 0, &back );

		if( back == NULL )
		{
			//_logger.message("Atlas::loadTexture back == NULL"
			//	);

			return false;
		}

		DXCALL m_pDevice->SetRenderTarget( 0, dest );

		DXCALL m_pDevice->BeginScene();
				
		this->renderSprite_( c, _texture );

		DXCALL m_pDevice->EndScene();
		
		DXCALL m_pDevice->SetRenderTarget( 0, back );
		back->Release();
		dest->Release();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Atlas::clearTexture( const MAGIC_CHANGE_ATLAS & c )
	{
		uint32_t index = s_imageIndex( c );

		TMapImages::iterator it_found = m_images.find( index );

		if( it_found == m_images.end() )
		{
			//_logger.message("Atlas::clearTexture not found!"
			//	);

			return false;
		}

		ImageDesc & desc = it_found->second;

		desc.texture->Release();
		
		m_images.erase( it_found );

		IDirect3DSurface9 * back = NULL;
		DXCALL m_pDevice->GetRenderTarget( 0, &back );

		if( back == NULL )
		{
			return false;
		}

		IDirect3DSurface9 * dest = NULL;
		m_pTexture->GetSurfaceLevel( 0, &dest );

		if( dest == NULL )
		{
			return false;
		}

		DXCALL m_pDevice->SetRenderTarget( 0, dest );

		D3DSURFACE_DESC texDesc;
		DXCALL m_pTexture->GetLevelDesc( 0, &texDesc );

		float cw = (float)c.width;
		float ch = (float)c.height;

		float tw = (float)texDesc.Width;
		float th = (float)texDesc.Height;

		float scale_x = tw / cw;
		float scale_y = th / ch;

		D3DRECT rect[1];
		rect[0].x1 = (LONG)( (c.x * scale_x) + 0.5f );
		rect[0].y1 = (LONG)((c.y * scale_y) + 0.5f);
		rect[0].x2 = (LONG)((c.x + c.width) * scale_x + 0.5f);
		rect[0].y2 = (LONG)((c.y + c.height) * scale_y + 0.5f);

		m_pDevice->Clear( 1, rect, D3DCLEAR_TARGET, 0x0, 0.f, 0 );
		
		dest->Release();

		m_pDevice->SetRenderTarget( 0, back );
		back->Release();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Atlas::beginRender( int _stage )
	{
		DXCALL m_pDevice->SetTexture( _stage, m_pTexture );
	}
}