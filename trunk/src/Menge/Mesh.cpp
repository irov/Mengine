#	include "Mesh.h" 

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Mesh::Mesh()
		: m_materialGroup(nullptr)
		, m_material(nullptr)
		, m_texturesNum(0)
		, m_blendAdd(false)
		, m_solid(false)
		, m_uvRotate(false)
        , m_invalidateMaterial(true)
		, m_invalidateVerticesLocal(true)
		, m_invalidateUVLocal(true)
		, m_invalidateVerticesWM(true)
		, m_invalidateVerticesColor(true)
		, m_vertexCount(0)
		, m_indicesCount(0)
		, m_textureUVOffset(0.f, 0.f)
		, m_textureUVScale(1.f, 1.f)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Mesh::~Mesh()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh::_compile()
	{
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
	bool Mesh::compileResource_()
	{
		if( m_resourceImage == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)( "Mesh::compileResource_ '%s' image resource null"
                , m_name.c_str() 
                );

            return false;
        }

        if( m_resourceImage.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Mesh::compileResource_ '%s' image resource %s not compile"
                , m_name.c_str() 
                , m_resourceImage->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::_release()
	{
        m_resourceImage.release();

        m_textures[0] = nullptr;
        m_textures[1] = nullptr;
		
		m_materialGroup = nullptr;
		m_material = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    void Mesh::setResourceImage( ResourceImage * _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        m_resourceImage = _resourceImage;

        this->recompile();

        //this->invalidateVertices_();
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImage * Mesh::getResourceImage() const
    {        
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh::updateResource_()
    {
        //bool uvRotate = m_resourceImage->isUVRotate();
        //this->setUVRotate( uvRotate );

        //const mt::vec4f & uv = m_resourceImage->getUVImage();
        //this->setUV( uv );

        //const mt::vec4f & uvAlpha = m_resourceImage->getUVAlpha();
        //this->setUV2( uvAlpha );

        m_textures[0] = m_resourceImage->getTexture();
        m_textures[1] = m_resourceImage->getTextureAlpha();
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh::invalidateMaterial()
    {
        m_invalidateMaterial = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Mesh::updateMaterial()
	{
        m_invalidateMaterial = false;

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTextureAlpha();

        if( textureAlpha != nullptr )
        {
            if( m_resourceImage->isAlpha() == true || m_solid == false )
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
			LOGGER_ERROR(m_serviceProvider)("Mesh::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		if( m_vertexCount == 0 )
		{
			return;
		}

		const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterial * material = this->getMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject( _camera, material, m_textures, m_texturesNum, PT_TRIANGLELIST, vertices, m_vertexCount, m_indices, m_indicesCount );
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const RenderVertex2D * vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( size_t i = 1; i != m_vertexCount; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::setTextureUVOffset( const mt::vec2f & _offset )
	{
		m_textureUVOffset = _offset;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Mesh::getTextureUVOffset() const
	{
		return m_textureUVOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::setTextureUVScale( const mt::vec2f & _scale )
	{
		m_textureUVScale = _scale;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Mesh::getTextureUVScale() const
	{
		return m_textureUVScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::setUVRotate( bool _rotate )
	{
		m_uvRotate = _rotate;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh::getUVRotate() const
	{
		return m_uvRotate;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::invalidateVertices()
	{
		m_invalidateVerticesLocal = true;

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::updateVertices()
	{
		m_invalidateVerticesLocal = false;

		const mt::vec4f & image_uv = m_resourceImage->getUVImage();
		const mt::vec4f & image_uvAlpha = m_resourceImage->getUVAlpha();

		if( m_uvRotate == false )
		{
			// RGB(A)
			{
				for( size_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_uvLocal[i];

					float u = image_uv.x + (image_uv.z - image_uv.x) * uv.x;
					float v = image_uv.y + (image_uv.w - image_uv.y) * uv.y;

					m_verticesWM[i].uv.x = u * m_textureUVScale.x + m_textureUVOffset.x;
					m_verticesWM[i].uv.y = v * m_textureUVScale.y + m_textureUVOffset.y;
				}
			}

			// Alpha
			//if( m_textures[1] != nullptr )
			{
				for( size_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_uvLocal[i];

					float u = image_uvAlpha.x + (image_uvAlpha.z - image_uvAlpha.x) * uv.x;
					float v = image_uvAlpha.y + (image_uvAlpha.w - image_uvAlpha.y) * uv.y;

					m_verticesWM[i].uv2.x = u * m_textureUVScale.x + m_textureUVOffset.x;
					m_verticesWM[i].uv2.y = v * m_textureUVScale.y + m_textureUVOffset.y;
				}
			}
		}
		else
		{
			// RGB(A)
			{
				for( size_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_uvLocal[i];

					float u = image_uv.x + (image_uv.z - image_uv.x) * uv.y;
					float v = image_uv.y + (image_uv.w - image_uv.y) * uv.x;

					m_verticesWM[i].uv.x = u * m_textureUVScale.x + m_textureUVOffset.x;
					m_verticesWM[i].uv.y = v * m_textureUVScale.y + m_textureUVOffset.y;
				}
			}

			// Alpha
			//if( m_textures[1] != nullptr )
			{
				for( size_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_uvLocal[i];

					float u = image_uvAlpha.x + (image_uvAlpha.z - image_uvAlpha.x) * uv.y;
					float v = image_uvAlpha.y + (image_uvAlpha.w - image_uvAlpha.y) * uv.x;

					m_verticesWM[i].uv2.x = u * m_textureUVScale.x + m_textureUVOffset.x;
					m_verticesWM[i].uv2.y = v * m_textureUVScale.y + m_textureUVOffset.y;
				}
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Mesh::_invalidateColor()
    {
        Node::_invalidateColor();

		this->invalidateVerticesColor();

        bool solid = this->isSolidColor();

        if( m_solid != solid )
        {
			m_solid = solid;

            this->invalidateMaterial();
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Mesh::invalidateVerticesColor()
	{
		m_invalidateVerticesColor = true;                
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::updateVerticesColor()
	{
		m_invalidateVerticesColor = false;

		ColourValue color;
		this->calcTotalColor(color);

		uint32 argb = color.getAsARGB();

		for( RenderVertex2D
			*it = m_verticesWM,
			*it_end = m_verticesWM + m_vertexCount;
		it != it_end;
		++it )
		{
			RenderVertex2D & vtx = *it;
			
			vtx.color = argb;
		}
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::setBlendAdd( bool _value )
	{
		if( m_blendAdd == _value )
		{
			return;
		}

		m_blendAdd = _value;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh::isBlendAdd() const
	{
		return m_blendAdd;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::setVerticies( const mt::vec3f * _position, const mt::vec2f * _uv, size_t _countVertex, const uint16 * _indicies, size_t _countIndex )
	{
		if( _countVertex >= MENGINE_MESH_MAX_VERTEX || _countIndex >= MENGINE_MESH_MAX_INDECIES )
		{
			LOGGER_ERROR(m_serviceProvider)("Mesh::setVerticies _countVertex %d >= 16 or _countIndex %d >= 42"
				, _countVertex
				, _countIndex
				);

			return;
		}

		memcpy( m_verticesLocal, _position, sizeof(mt::vec3f) * _countVertex );
		memcpy( m_uvLocal, _uv, sizeof(mt::vec2f) * _countVertex );

		memcpy( m_indices, _indicies, sizeof(uint16) * _countIndex );

		m_vertexCount = _countVertex;
		m_indicesCount = _countIndex;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh::updateVerticesWM()
	{
		m_invalidateVerticesWM = false;

		if( m_invalidateVerticesLocal == true )
		{
			this->updateVertices();
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		for( size_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec3f & pos = m_verticesLocal[i];

			mt::vec3f & wm_pos = m_verticesWM[i].pos;
			mt::mul_v3_m4( wm_pos, pos, wm);
		}
	}
}
