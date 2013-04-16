#	include "Sprite.h" 
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

//#	 include "ResourceTexture.h"
//#	include "Kernel/Affector.h"

namespace	Menge
{
	enum ESpriteVerticesInvalidate
	{
		ESVI_TRANSFORM = 0x01,
		ESVI_COLOR = 0x02,
		ESVI_MATERIAL = 0x04,
		ESVI_TEXTURE = 0x08,
		ESVI_FULL = 0xFF
	};
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
		: m_resource(0)
		, m_flipX(false)
		, m_flipY(false)
		, m_percentVisibility(0.f, 0.f, 0.f, 0.f)
		, m_materialGroup(NULL)
		, m_material(NULL)
		, m_disableTextureColor(false)
		, m_texturesNum(0)
		, m_textureMatrixOffset(0.f, 0.f)
        , m_textureUVOffset(0.f, 0.f)
        , m_textureUVScale(1.f, 1.f)
		, m_blendAdd(false)
		, m_solid(false)
		, m_isCustomSize(false)
		, m_spriteSize(0.f, 0.f)
        , m_invalidateVerticesLocal(0xFF)
        , m_invalidateVerticesWM(true)
	{ 
		m_textures[0] = NULL;
		m_textures[1] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{
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
    
		this->invalidateVertices_();
		this->invalidateBoundingBox();

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

		if( m_resourceName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)( "Sprite.compileResource_: '%s' Image resource empty"
				, m_name.c_str() 
				);

			return false;
		}

		m_resource = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImage>( m_resourceName );

		if( m_resource == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Sprite: '%s' Image resource not found resource '%s'"
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
		
		m_materialGroup = NULL;
		m_material = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setFlipX( bool _flipX )
	{
		m_flipX = _flipX;

		this->invalidateVertices_();
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setFlipY( bool _flipY )
	{
		m_flipY = _flipY;

		this->invalidateVertices_();
		this->invalidateBoundingBox();
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
	void Sprite::setImageResourceName( const ConstString& _name )
	{
		if( m_resourceName == _name )
		{
			return;
		}

		m_resourceName = _name;

		this->recompile();

		this->invalidateVertices_();
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Sprite::getImageResourceName() const
	{
		return m_resourceName;
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceImage * Sprite::getImageResource() const
    {
        return m_resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::updateVerticesWM_()
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVerticesLocal != 0 )
        {
            this->updateVertices_();

            m_invalidateVerticesLocal = 0;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( size_t i = 0; i != 4; ++i )
        {
            const mt::vec3f & pos = m_verticesLocal[i];

            mt::vec3f & wm_pos = m_verticesWM[i].pos;
            mt::mul_v3_m4( wm_pos, pos, wm);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::invalidateVertices_( unsigned char _invalidate )
    {
        m_invalidateVerticesLocal |= _invalidate;

        this->invalidateVerticesWM_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::invalidateVerticesWM_()
    {
        m_invalidateVerticesWM = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateVertices_()
	{
		if( m_resource == 0 )
		{
			return;
		}

		bool wrapX = m_resource->isWrapX();
		bool wrapY = m_resource->isWrapY();

		if( m_invalidateVerticesLocal & ESVI_TEXTURE )
		{
			m_textures[0] = m_resource->getTexture();
            m_textures[1] = m_resource->getTextureAlpha();
		}

		if( m_invalidateVerticesLocal & ESVI_TRANSFORM )
		{
			mt::vec2f size;
			mt::vec2f maxSize;

			if( m_isCustomSize == false )
			{
				size = m_resource->getSize();
				maxSize = m_resource->getMaxSize();
			}
			else
			{
				size = m_spriteSize;
				maxSize = m_spriteSize;
			}
			
			mt::vec4f percentPx( m_percentVisibility.x * maxSize.x, m_percentVisibility.y * maxSize.y,
				m_percentVisibility.z * maxSize.x, m_percentVisibility.w * maxSize.y );
				
			percentPx.z -= (maxSize.x - size.x);
			percentPx.w -= (maxSize.y - size.y);

			if( wrapX == false && wrapY == false )
			{
				if( percentPx.x < 0.f )
				{
					percentPx.x = 0.f;
				}
				else if( percentPx.x > size.x )
				{
					percentPx.x = size.x;
				}

				if( percentPx.y < 0.f )
				{
					percentPx.y = 0.f;
				}
				else if( percentPx.y > size.y )
				{
					percentPx.y = size.y;
				}

				if( percentPx.z < 0.f )
				{
					percentPx.z = 0.f;
				}
				else if( percentPx.z > size.x )
				{
					percentPx.z = size.x;
				}

				if( percentPx.w < 0.f )
				{
					percentPx.w = 0.f;
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

			if( m_invalidateVerticesLocal == ESVI_FULL )
			{
                bool isRotate = m_resource->isUVRotate();

                if( isRotate == false )
                {
                    // RGB(A)
                    {
                        mt::vec4f uv = m_resource->getUVImage();

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

                        m_verticesWM[0].uv.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[0].uv.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[1].uv.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[1].uv.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[2].uv.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[2].uv.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[3].uv.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[3].uv.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                    }

                    // Alpha
                    if( m_textures[1] != NULL )
                    {
                        mt::vec4f uv = m_resource->getUVAlpha();

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

                        m_verticesWM[0].uv2.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[0].uv2.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[1].uv2.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[1].uv2.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[2].uv2.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[2].uv2.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[3].uv2.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[3].uv2.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                    }
                    else
                    {
                        m_verticesWM[0].uv2.x = 0.f;
                        m_verticesWM[0].uv2.y = 0.f;
                        m_verticesWM[1].uv2.x = 0.f;
                        m_verticesWM[1].uv2.y = 0.f;
                        m_verticesWM[2].uv2.x = 0.f;
                        m_verticesWM[2].uv2.y = 0.f;
                        m_verticesWM[3].uv2.x = 0.f;
                        m_verticesWM[3].uv2.y = 0.f;
                    }
                }
                else
                {
                    // RGB(A)
                    {
                        mt::vec4f uv = m_resource->getUVImage();

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

                        m_verticesWM[0].uv.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[0].uv.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[1].uv.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[1].uv.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[2].uv.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[2].uv.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[3].uv.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[3].uv.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                    }

                    // Alpha
                    if( m_textures[1] != NULL )
                    {
                        mt::vec4f uv = m_resource->getUVAlpha();

                        float uvX = uv.z - uv.x;
                        float uvY = uv.w - uv.y;

                        uv.x += percent.y * uvY;
                        uv.y += percent.x * uvX;
                        uv.z -= percent.w * uvY;
                        uv.w -= percent.z * uvX;                        
                        
                        if( m_flipX == true )
                        {
                            std::swap( uv.y, uv.w );
                        }

                        if( m_flipY == true )
                        {
                            std::swap( uv.x, uv.z );
                        }

                        m_verticesWM[0].uv2.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[0].uv2.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[1].uv2.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[1].uv2.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[2].uv2.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[2].uv2.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                        m_verticesWM[3].uv2.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                        m_verticesWM[3].uv2.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                    }
                    else
                    {
                        m_verticesWM[0].uv2.x = 0.f;
                        m_verticesWM[0].uv2.y = 0.f;
                        m_verticesWM[1].uv2.x = 0.f;
                        m_verticesWM[1].uv2.y = 0.f;
                        m_verticesWM[2].uv2.x = 0.f;
                        m_verticesWM[2].uv2.y = 0.f;
                        m_verticesWM[3].uv2.x = 0.f;
                        m_verticesWM[3].uv2.y = 0.f;
                    }
                }
			}

			mt::vec2f visOffset( size.x * percent.x, size.y * percent.y );

			size.x -= size.x * ( percent.x + percent.z );
			size.y -= size.y * ( percent.y + percent.w );
             
            m_verticesLocal[0] = mt::vec3f(visOffset.x + 0.f, visOffset.y + 0.f, 0.f);
            m_verticesLocal[1] = mt::vec3f(visOffset.x + size.x, visOffset.y + 0.f, 0.f);
            m_verticesLocal[2] = mt::vec3f(visOffset.x + size.x, visOffset.y + size.y, 0.f);
            m_verticesLocal[3] = mt::vec3f(visOffset.x + 0.f, visOffset.y + size.y, 0.f);
		}

		if( m_invalidateVerticesLocal & ESVI_COLOR )
		{
			ColourValue color;
			this->calcTotalColor(color);

			uint32 argb = color.getAsARGB();

			ApplyColor2D applyColor( argb );
			std::for_each( m_verticesWM, m_verticesWM + 4, applyColor );
			
			bool solid = (( argb & 0xFF000000 ) == 0xFF000000 );

			if( m_solid != solid )
			{
				m_solid = solid;
				m_invalidateVerticesLocal |= ESVI_MATERIAL;
			}
		}

		if( m_invalidateVerticesLocal & ESVI_MATERIAL )
		{
			this->updateMaterial_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateMaterial_()
	{
        RenderTextureInterface * textureAlpha = m_resource->getTextureAlpha();

        if( textureAlpha != NULL )
        {
            m_texturesNum = 2;

            m_materialGroup = RENDER_SERVICE(m_serviceProvider)
                ->getMaterialGroup( CONST_STRING(m_serviceProvider, ExternalAlpha) );
        }
		else if ( m_blendAdd == true )
		{
			m_texturesNum = 1;

			m_materialGroup = RENDER_SERVICE(m_serviceProvider)
				->getMaterialGroup( CONST_STRING(m_serviceProvider, ParticleIntensive) );
		}
		else if( m_disableTextureColor == true )
		{
			m_texturesNum = 1;

			m_materialGroup = RENDER_SERVICE(m_serviceProvider)
				->getMaterialGroup( CONST_STRING(m_serviceProvider, OnlyColor) );
		}
		else
		{
			m_texturesNum = 1;

			if( m_resource->isAlpha() == true || m_solid == false )
			{
				m_materialGroup = RENDER_SERVICE(m_serviceProvider)
					->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );
			}
			else
			{
				m_materialGroup = RENDER_SERVICE(m_serviceProvider)
					->getMaterialGroup( CONST_STRING(m_serviceProvider, SolidSprite) );
			}
		}

        //m_resource = 0;

		bool wrapX = m_resource->isWrapX();
		bool wrapY = m_resource->isWrapY();

		ETextureAddressMode textureU = wrapX ? TAM_WRAP : TAM_CLAMP;
		ETextureAddressMode textureV = wrapY ? TAM_WRAP : TAM_CLAMP;

		m_material = m_materialGroup->getMaterial( textureU, textureV );

		if( m_material == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("Sprite::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );
		
		const Vertex2D * vertices = this->getVerticesWM_();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject2D( _camera, m_material, m_textures, m_texturesNum, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const Vertex2D * vertices = this->getVerticesWM_();

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

		this->invalidateVertices_( ESVI_COLOR );
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

		m_materialGroup = RENDER_SERVICE(m_serviceProvider)
			->getMaterialGroup( CONST_STRING(m_serviceProvider, OnlyColor) );

		this->invalidateVertices_( ESVI_MATERIAL );
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec4f& _percent )
	{
		m_percentVisibility = _percent;

		this->invalidateVertices_( ESVI_FULL );
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Sprite::getPercentVisibility() const
	{
		return m_percentVisibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setTextureMatrixOffset( const mt::vec2f& _offset )
	{
		m_textureMatrixOffset = _offset;

		this->invalidateVertices_( ESVI_TRANSFORM );
	}
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setTextureUVOffset( const mt::vec2f & _offset )
    {
        m_textureUVOffset = _offset;

        this->invalidateVertices_( ESVI_TRANSFORM );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Sprite::getTextureUVOffset() const
    {
        return m_textureUVOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setTextureUVScale( const mt::vec2f & _scale )
    {
        m_textureUVScale = _scale;

        this->invalidateVertices_( ESVI_TRANSFORM );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Sprite::getTextureUVScale() const
    {
        return m_textureUVScale;
    }
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setBlendAdd( bool _value )
	{
		if ( m_blendAdd == _value )
		{
			return;
		}

		m_blendAdd = _value;

		this->invalidateVertices_( ESVI_MATERIAL );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::isBlendAdd() const
	{
		return m_blendAdd;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setSpriteSize( const mt::vec2f& _size )
	{
		m_spriteSize = _size;
		m_isCustomSize = true;
	}
	//////////////////////////////////////////////////////////////////////////
}
