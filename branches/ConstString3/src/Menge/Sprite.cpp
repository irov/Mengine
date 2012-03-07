#	include "Sprite.h" 

#	include "Layer2D.h"

#	include "BinParser.h"

#	include "RenderEngine.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "Consts.h"

#	include "LogEngine.h"

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
		ESVI_TEXTURE = 0x08,
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
		, m_materialGroup(NULL)
		, m_material(NULL)
		, m_disableTextureColor(false)
		, m_texturesNum(0)
		, m_textureMatrixOffset(0.0f, 0.0f)
		, m_flexible(false)
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

		size_t max = m_resource->getCount();
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
			if( m_resource->compile() == false )
			{
				return false;
			}

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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		Node::_release();
	
		if( m_resource != 0 )
		{
			m_resource->decrementReference();
			m_resource = 0;
		}

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
	void Sprite::setImageIndex( size_t _index )
	{
		if( m_resource != NULL )
		{
			size_t max = m_resource->getCount();
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
	size_t Sprite::getImageIndex() const
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

		if( _invalidateVertices & ESVI_TEXTURE )
		{
			m_textures[0] = m_resource->getTexture( m_currentImageIndex );
		}

		if( _invalidateVertices & ESVI_POSITION && m_flexible == false )
		{
			mt::vec2f size = m_resource->getSize( m_currentImageIndex );
			const mt::vec2f& maxSize = m_resource->getMaxSize( m_currentImageIndex );
			mt::vec2f offset = m_resource->getOffset( m_currentImageIndex );

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
			//percent.x = ::floorf( percent.x * size.x + 0.5f ) / size.x;
			//percent.y = ::floorf( percent.y * size.y + 0.5f ) / size.y;
			//percent.z = ::floorf( percent.z * size.x + 0.5f ) / size.x;
			//percent.w = ::floorf( percent.w * size.y + 0.5f ) / size.y;

			if( _invalidateVertices == ESVI_FULL )
			{
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

			const mt::mat3f & wm = this->getWorldMatrix();

			//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
			//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
			//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
			//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
			mt::vec2f transformX;
			mt::vec2f transformY;

			mt::vec2f vertices[4];

			mt::mul_v2_m3( vertices[0], visOffset, wm );
			mt::mul_v2_m3_r( transformX, mt::vec2f(size.x, 0.f), wm );
			mt::mul_v2_m3_r( transformY, mt::vec2f(0.f, size.y), wm );

			vertices[1] = vertices[0] + transformX;
			vertices[2] = vertices[1] + transformY;
			vertices[3] = vertices[0] + transformY;

			for( int i = 0; i < 4; i++ )
			{
				_vertcies[i].pos[0] = vertices[i].x;
				_vertcies[i].pos[1] = vertices[i].y;
				_vertcies[i].pos[2] = 0.f;
				_vertcies[i].pos[3] = 1.f;
			}
		}

		if( _invalidateVertices & ESVI_COLOR )
		{
			ColourValue color;
			this->calcTotalColor(color);

			uint32 argb = color.getAsARGB();

			ApplyColor2D applyColor( argb );
			std::for_each( _vertcies, _vertcies + 4, applyColor );
		}

		if( _invalidateVertices & ESVI_MATERIAL )
		{
			updateMaterial_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateMaterial_()
	{
		if( m_disableTextureColor == true )
		{
			m_texturesNum = 1;

			m_materialGroup = RenderEngine::get()
				->getMaterialGroup( CONST_STRING(OnlyColor) );
		}
		else
		{
			m_texturesNum = 1;

			ColourValue color;
			this->calcTotalColor(color);

			uint32 argb = color.getAsARGB();

			bool solid = (( argb & 0xFF000000 ) == 0xFF000000 );

			if( m_resource->isAlpha( m_currentImageIndex ) == true || solid == false )
			{
				m_materialGroup = RenderEngine::get()
					->getMaterialGroup( CONST_STRING(BlendSprite) );
			}
			else
			{
				m_materialGroup = RenderEngine::get()
					->getMaterialGroup( CONST_STRING(SolidSprite) );
			}
		}

		bool wrapX = m_resource->getWrapX( m_currentImageIndex );
		bool wrapY = m_resource->getWrapY( m_currentImageIndex );

		ETextureAddressMode textureU = wrapX ? TAM_WRAP : TAM_CLAMP;
		ETextureAddressMode textureV = wrapY ? TAM_WRAP : TAM_CLAMP;

		m_material = m_materialGroup->getMaterial( textureU, textureV );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( Camera2D * _camera )
	{
		Node::_render( _camera );


		const Vertex2D * vertices = this->getVertices();

		bool scaled = this->isScaled();

		RenderEngine::get()
			->renderObject2D( m_material, m_textures, m_textureMatrix, m_texturesNum, vertices, 4, scaled, LPT_QUAD );
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
		const Vertex2D * vertices = getVertices();

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

		invalidateVertices( ESVI_COLOR | ESVI_MATERIAL );
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

			return mt::zero_v2;
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
	size_t Sprite::getImageCount()
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
	void Sprite::setPercentVisibility( const mt::vec4f& _percent )
	{
		m_percent = _percent;

		invalidateVertices();
		invalidateBoundingBox();
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
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setFlexible( bool _value )
	{
		m_flexible = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getFlexible() const
	{
		return m_flexible;
	}
}
