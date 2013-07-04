#	include "Sprite.h" 
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
		: m_resourceImage(nullptr)
		, m_materialGroup(nullptr)
		, m_material(nullptr)
		, m_disableTextureColor(false)
		, m_texturesNum(0)
		, m_blendAdd(false)
		, m_solid(false)
		, m_isCustomSize(false)
		, m_customSize(0.f, 0.f)
        , m_invalidateMaterial(true)
	{ 
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

        this->updateResource_();
        
        this->invalidateMaterial();
		this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::compileResource_()
	{
		if( m_resourceImage != nullptr )
		{
			if( m_resourceImage->compile() == false )
			{
				return false;
			}

			return true;
		}

		if( m_resourceImageName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)( "Sprite.compileResource_: '%s' Image resource empty"
				, m_name.c_str() 
				);

			return false;
		}

		m_resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImage>( m_resourceImageName );

		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Sprite: '%s' Image resource not found resource '%s'"
				, m_name.c_str()
				, m_resourceImageName.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		Node::_release();

		if( m_resourceImage != nullptr )
		{
			m_resourceImage->decrementReference();
			m_resourceImage = nullptr;
		}

        m_textures[0] = nullptr;
        m_textures[1] = nullptr;
		
		m_materialGroup = nullptr;
		m_material = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResourceName( const ConstString& _name )
	{
		if( m_resourceImageName == _name )
		{
			return;
		}

		m_resourceImageName = _name;

		this->recompile();

		//this->invalidateVertices_();
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Sprite::getImageResourceName() const
	{
		return m_resourceImageName;
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceImage * Sprite::getImageResource() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::updateResource_()
    {
        bool wrapX = m_resourceImage->isWrapX();
        this->setTextureWrapX( wrapX );

        bool wrapY = m_resourceImage->isWrapY();
        this->setTextureWrapY( wrapY );

        if( m_isCustomSize == false )
        {
            const mt::vec2f & size = m_resourceImage->getSize();
            this->setSize( size );

            const mt::vec2f & maxSize = m_resourceImage->getMaxSize();
            this->setMaxSize( maxSize );
        }
        else
        {
            this->setSize( m_customSize );
            this->setMaxSize( m_customSize );
        }

        bool uvRotate = m_resourceImage->isUVRotate();
        this->setUVRotate( uvRotate );

        const mt::vec4f & uv = m_resourceImage->getUVImage();
        this->setUV( uv );

        const mt::vec4f & uvAlpha = m_resourceImage->getUVAlpha();
        this->setUV2( uvAlpha );

        m_textures[0] = m_resourceImage->getTexture();
        m_textures[1] = m_resourceImage->getTextureAlpha();
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::invalidateMaterial()
    {
        m_invalidateMaterial = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateMaterial()
	{
        m_invalidateMaterial = false;

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTextureAlpha();

        if( textureAlpha != nullptr )
        {
            if( m_disableTextureColor == true )
            {
                m_texturesNum = 2;

                m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
                    ->getMaterialGroup( CONST_STRING(m_serviceProvider, ExternalAlpha_OnlyColor) );
            }
            else if( m_resourceImage->isAlpha() == true || m_solid == false )
            {
                m_texturesNum = 2;

                m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
                    ->getMaterialGroup( CONST_STRING(m_serviceProvider, ExternalAlpha) );
            }
            else
            {
                m_texturesNum = 1;

                m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
                    ->getMaterialGroup( CONST_STRING(m_serviceProvider, SolidSprite) );
            }
        }
		else if( m_blendAdd == true )
		{
			m_texturesNum = 1;

			m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
				->getMaterialGroup( CONST_STRING(m_serviceProvider, ParticleIntensive) );
		}
		else if( m_disableTextureColor == true )
		{
			m_texturesNum = 1;

			m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
				->getMaterialGroup( CONST_STRING(m_serviceProvider, OnlyColor) );
		}
		else
		{
			m_texturesNum = 1;

			if( m_resourceImage->isAlpha() == true || m_solid == false )
			{
				m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
					->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );
			}
			else
			{
				m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
					->getMaterialGroup( CONST_STRING(m_serviceProvider, SolidSprite) );
			}
		}

		bool wrapX = m_resourceImage->isWrapX();
		bool wrapY = m_resourceImage->isWrapY();

		ETextureAddressMode textureU = wrapX ? TAM_WRAP : TAM_CLAMP;
		ETextureAddressMode textureV = wrapY ? TAM_WRAP : TAM_CLAMP;

		m_material = m_materialGroup->getMaterial( textureU, textureV );

		if( m_material == nullptr )
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
		
		const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterial * material = this->getMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject2D( _camera, material, m_textures, m_texturesNum, LPT_QUAD, vertices, 4 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const RenderVertex2D * vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Sprite::_invalidateColor()
    {
        Shape::_invalidateColor();

        bool solid = this->isSolidColor();

        if( m_solid != solid )
        {
            this->invalidateMaterial();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setCustomSize( const mt::vec2f & _size )
    {
        m_customSize = _size;
        m_isCustomSize = true;

        if( m_resourceImage != nullptr )
        {
            this->updateResource_();
        }
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

		m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterialGroup( CONST_STRING(m_serviceProvider, OnlyColor) );

        this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setBlendAdd( bool _value )
	{
		if ( m_blendAdd == _value )
		{
			return;
		}

		m_blendAdd = _value;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::isBlendAdd() const
	{
		return m_blendAdd;
	}
}
