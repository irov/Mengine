#	include "Sprite.h" 

#	include "Layer2D.h"

#	include "BinParser.h"

#	include "RenderEngine.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

//#	 include "ResourceTexture.h"
#	include "Material.h"
#	include "Affector.h"
#	include "Texture.h"

namespace	Menge
{
	enum ESpriteVerticesInvalidate
	{
		ESVI_POSITION = 0x01,
		ESVI_COLOR = 0x02,
		ESVI_MATERIAL = 0x04,
		ESVI_FULL = 0xFF
	};
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
		: m_resource(0)
		, m_currentImageIndex(0)
		, m_currentAlphaImageIndex(0)
		, m_centerAlign(false)
		, m_percent(0.0f, 0.0f, 0.0f, 0.0f)
		, m_flipX(false)
		, m_flipY(false)
		, m_solid(false)
		, m_materialGroup(NULL)
		, m_material(NULL)
		, m_alphaImage(NULL)
		, m_disableTextureColor(false)
		, m_texturesNum(0)
		, m_textureMatrixOffset(0.0f, 0.0f)
	{ 
		m_textures[0] = NULL;
		m_textures[1] = NULL;

		m_textureMatrix[0] = NULL;
		m_textureMatrix[1] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::ImageMap_Name, m_resourceName );
			BIN_CASE_ATTRIBUTE( Protocol::ImageAlpha_Name, m_alphaImageName );
			BIN_CASE_ATTRIBUTE( Protocol::ImageIndex_Value, m_currentImageIndex );
			BIN_CASE_ATTRIBUTE( Protocol::CenterAlign_Value, m_centerAlign );
			BIN_CASE_ATTRIBUTE( Protocol::PercentVisibility_Value, m_percent );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		if( this->compileResource_() == false )
		{
			return false;
		}

		std::size_t max = m_resource->getCount();
		if( m_currentImageIndex >= max )
		{
			MENGE_LOG_WARNING( "Sprite: %s compile index (%d) >= image count(%d)"
				, m_name.c_str()
				, m_currentImageIndex
				, max
				);

			return false;
		}

		invalidateVertices();
		invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::compileResource_()
	{
		if( m_resource )
		{
			return true;
		}

		if( m_resourceName.empty() )
		{
			MENGE_LOG_ERROR( "Sprite: '%s' Image resource empty"
				, m_name.c_str() 
				);

			return false;
		}

		m_resource = ResourceManager::get()
			->getResourceT<ResourceImage>( m_resourceName );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Sprite: '%s' Image resource not found '%s'"
				, m_name.c_str()
				, m_resourceName.c_str() 
				);

			return false;
		}

		m_texturesNum = 1;

		if( m_disableTextureColor == true )
		{
			m_materialGroup = RenderEngine::get()
				->getMaterialGroup( CONST_STRING(OnlyColor) );
		}
		else
		{
			m_materialGroup = RenderEngine::get()
				->getMaterialGroup( CONST_STRING(Sprite) );
		}

		if( m_alphaImageName.empty() == true )
		{
			return true;
		}

		m_alphaImage = ResourceManager::get()
			->getResourceT<ResourceImage>( m_alphaImageName );

		if( m_alphaImage == NULL )
		{
			MENGE_LOG_ERROR( "Sprite: '%s' can't get AlphaImage '%s'"
				, m_name.c_str()
				, m_alphaImageName.c_str() 
				);

			return false;
		}

		m_texturesNum = 2;

		m_materialGroup = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(ExternalAlpha) );
	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		Node::_release();

		ResourceManager::get()
			->releaseResource( m_alphaImage );

		m_alphaImage = 0;

		ResourceManager::get()
			->releaseResource( m_resource );

		m_resource = 0;

		for( int i = 0; i != 2; ++i )
		{
			delete m_textureMatrix[i];
			m_textureMatrix[i] = NULL;
		}

		m_materialGroup = NULL;
		m_material = NULL;

		m_resource = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setFlipX( bool _flipX )
	{
		m_flipX = _flipX;

		invalidateVertices();
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setFlipY( bool _flipY )
	{
		m_flipY = _flipY;

		invalidateVertices();
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getFlipX() const
	{
		return m_flipX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getFlipY() const
	{
		return m_flipY;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageIndex( std::size_t _index )
	{
		if( m_resource != NULL )
		{
			std::size_t max = m_resource->getCount();
			if( _index >= max )
			{
				MENGE_LOG_WARNING( "Warning: (Sprite::setImageIndex) index (%d) >= image count(%d)"
					, _index
					, max
					);

				return;
			}
		}

		m_currentImageIndex = _index;

		invalidateVertices();
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageIndex() const
	{
		return m_currentImageIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResource( const ConstString& _name )
	{
		if( m_resourceName == _name )
		{
			return;
		}

		m_resourceName = _name;

		m_currentImageIndex = 0; //?? wtf

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& Sprite::getImageResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateVertices( Vertex2D * _vertcies, unsigned char _invalidateVertices )
	{
		if( m_resource == 0 ) 
		{
			return;
		}

		bool wrapX = m_resource->getWrapX( m_currentImageIndex );
		bool wrapY = m_resource->getWrapY( m_currentImageIndex );

		if( _invalidateVertices & ESVI_POSITION )
		{
			ResourceImage * resource = m_alphaImage ? m_alphaImage : m_resource;

			mt::vec2f size = resource->getSize( m_currentImageIndex );
			const mt::vec2f& maxSize = resource->getMaxSize( m_currentImageIndex );
			mt::vec2f offset = resource->getOffset( m_currentImageIndex );

			mt::vec4f percentPx( m_percent.x * maxSize.x, m_percent.y * maxSize.y,
				m_percent.z * maxSize.x, m_percent.w * maxSize.y );

			percentPx.x -= offset.x;
			percentPx.y -= offset.y;
			percentPx.z -= (maxSize.x - offset.x - size.x);
			percentPx.w -= (maxSize.y - offset.y - size.y);

			if( wrapX == false && wrapY == false )
			{
				if( percentPx.x < 0.0f )
				{
					percentPx.x = 0.0f;
				}
				else if( percentPx.x > size.x )
				{
					percentPx.x = size.x;
				}
				if( percentPx.y < 0.0f )
				{
					percentPx.y = 0.0f;
				}
				else if( percentPx.y > size.y )
				{
					percentPx.y = size.y;
				}
				if( percentPx.z < 0.0f )
				{
					percentPx.z = 0.0f;
				}
				else if( percentPx.z > size.x )
				{
					percentPx.z = size.x;
				}
				if( percentPx.w < 0.0f )
				{
					percentPx.w = 0.0f;
				}
				else if( percentPx.w > size.y )
				{
					percentPx.w = size.y;
				}
			}

			mt::vec4f percent( percentPx.x / size.x, percentPx.y / size.y,
				percentPx.z / size.x, percentPx.w / size.y );

			// adjust texture visibility
			percent.x = ::floorf( percent.x * size.x + 0.5f ) / size.x;
			percent.y = ::floorf( percent.y * size.y + 0.5f ) / size.y;
			percent.z = ::floorf( percent.z * size.x + 0.5f ) / size.x;
			percent.w = ::floorf( percent.w * size.y + 0.5f ) / size.y;

			if( _invalidateVertices == ESVI_FULL )
			{
				m_textures[0] = m_resource->getTexture( m_currentImageIndex );

				mt::vec4f uv = m_resource->getUV( m_currentImageIndex );

				float uvX = uv.z - uv.x;
				float uvY = uv.w - uv.y;

				uv.x += percent.x * uvX;
				uv.y += percent.y * uvY;
				uv.z -= percent.z * uvX;
				uv.w -= percent.w * uvY;

				if( m_flipX == true )
				{
					std::swap( uv.x, uv.z );
				}

				if( m_flipY == true )
				{
					std::swap( uv.y, uv.w );
				}

				_vertcies[0].uv[0] = uv.x;
				_vertcies[0].uv[1] = uv.y;
				_vertcies[1].uv[0] = uv.z;
				_vertcies[1].uv[1] = uv.y;
				_vertcies[2].uv[0] = uv.z;
				_vertcies[2].uv[1] = uv.w;
				_vertcies[3].uv[0] = uv.x;
				_vertcies[3].uv[1] = uv.w;
			}

			mt::vec2f visOffset( size.x * percent.x, size.y * percent.y );

			size.x -= size.x * ( percent.x + percent.z );
			size.y -= size.y * ( percent.y + percent.w );

			if( m_centerAlign )
			{
				mt::vec2f alignOffset = maxSize * -0.5f;
				alignOffset.x = ::floorf( alignOffset.x + 0.5f );
				alignOffset.y = ::floorf( alignOffset.y + 0.5f );

				visOffset += alignOffset;
			}

			if( m_flipX )
			{
				offset.x = maxSize.x - ( size.x + offset.x );
			}

			if( m_flipY )
			{
				offset.y = maxSize.y - ( size.y + offset.y );
			}

			visOffset += offset;

			if( m_alphaImage )
			{
				m_textures[1] = m_alphaImage->getTexture( m_currentAlphaImageIndex );

				const mt::vec2f& rgbSize = m_resource->getSize( m_currentImageIndex );

				if( rgbSize.x > size.x || rgbSize.y > size.y )
				{
					if( m_textureMatrix[0] == NULL )
					{
						m_textureMatrix[0] = new mt::mat4f();
					}

					mt::mat4f * texMat = m_textureMatrix[0];
					mt::ident_m4( *texMat );
					texMat->v0.x = size.x / rgbSize.x;
					texMat->v1.y = size.y / rgbSize.y;

					Texture * rgbTexture = m_resource->getTexture( m_currentImageIndex );

					texMat->v2.x = ( offset.x + m_textureMatrixOffset.x ) / rgbTexture->getHWWidth();		// ugly place :( We must not know about HW sizes of
					texMat->v2.y = ( offset.y + m_textureMatrixOffset.y ) / rgbTexture->getHWHeight();	// texture here, either about texture matrix
				}
			}

			const mt::mat3f & wm = this->getWorldMatrix();

			//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
			//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
			//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
			//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
			mt::vec2f transformX;
			mt::vec2f transformY;

			mt::vec2f vertices[4];

			mt::mul_v2_m3( vertices[0], visOffset, wm );
			mt::mul_v2_m3_r( transformX, mt::vec2f( size.x, 0.0f ), wm );
			mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, size.y ), wm );

			vertices[1] = vertices[0] + transformX;
			vertices[2] = vertices[1] + transformY;
			vertices[3] = vertices[0] + transformY;

			for( int i = 0; i < 4; i++ )
			{
				_vertcies[i].pos[0] = vertices[i].x;
				_vertcies[i].pos[1] = vertices[i].y;
			}
		}

		if( _invalidateVertices & ESVI_COLOR )
		{
			const ColourValue & colour = this->getWorldColor();
			uint32 argb = colour.getAsARGB();

			ApplyColor2D applyColor( argb );
			std::for_each( _vertcies, _vertcies + 4, applyColor );

			m_solid = (( argb & 0xFF000000 ) == 0xFF000000 );
		}

		if( _invalidateVertices & ESVI_MATERIAL )
		{
			ETextureAddressMode textureU = wrapX?TAM_WRAP:TAM_CLAMP;
			ETextureAddressMode textureV = wrapY?TAM_WRAP:TAM_CLAMP;

			m_material = m_materialGroup->getMaterial( textureU, textureV );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		Vertex2D * vertices = this->getVertices();

		RenderEngine::get()
			->renderObject2D( m_material, m_textures, m_textureMatrix, m_texturesNum, vertices, 4, LPT_QUAD, m_solid );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		invalidateVertices( ESVI_POSITION );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Vertex2D * vertices = getVertices();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateColor()
	{
		Node::_invalidateColor();

		invalidateVertices( ESVI_COLOR );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Sprite::getImageSize()
	{
		if( this->compileResource_() == false )
		{
			MENGE_LOG_ERROR( "Sprite %s: Can't get image size, because resource is NULL '%s'"
				, getName().c_str()
				, m_resourceName.c_str()
				);

			return mt::vec2f::zero_v2;
		}

		const mt::vec2f & size = 
			m_resource->getMaxSize( m_currentImageIndex );
		//m_resource->getSize( m_currentImageIndex );

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getCenterAlign() const
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageCount()
	{
		if( this->compileResource_() == false )
		{
			MENGE_LOG_ERROR( "Sprite %s: Can't get image count, because resource is NULL '%s'"
				, getName().c_str()
				, m_resourceName.c_str() 
				);

			return 0;
		}

		return m_resource->getCount();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setCenterAlign( bool _centerAlign )
	{
		m_centerAlign = _centerAlign;

		invalidateVertices( ESVI_POSITION );
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageAlpha( const ConstString& _name )
	{
		if( m_alphaImageName == _name )
		{
			return;
		}

		m_alphaImageName = _name;

		recompile();

		//invalidateVertices();
		//invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageAlphaIndex( std::size_t _index )
	{
		if( m_alphaImage == NULL )
		{
			return;
		}

		size_t lastIndex = m_alphaImage->getCount() - 1;
		m_currentAlphaImageIndex = mt::clamp<size_t>( 0, _index, lastIndex );
		m_textures[1] = m_alphaImage->getTexture( m_currentAlphaImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::disableTextureColor( bool _disable )
	{
		if( m_disableTextureColor == _disable )
		{
			return;
		}

		m_disableTextureColor = _disable;

		if( this->isCompile() == false )
		{
			return;
		}

		m_materialGroup = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(OnlyColor) );

		invalidateVertices( ESVI_MATERIAL );
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY )
	{
		m_percent = mt::vec4f( _percentX, _percentY );

		invalidateVertices();
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityVec4f( const mt::vec4f& _percent )
	{
		mt::vec2f begin( _percent.x, _percent.y );
		mt::vec2f end( _percent.x, _percent.y );

		setPercentVisibility( begin, end );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Sprite::getPercentVisibility() const
	{
		return m_percent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setTextureMatrixOffset( const mt::vec2f& _offset )
	{
		m_textureMatrixOffset = _offset;

		invalidateVertices( ESVI_POSITION );
	}
}
