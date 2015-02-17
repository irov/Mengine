#	include "Mesh2D.h" 

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Mesh2D::Mesh2D()
		: m_material(nullptr)
		, m_blendAdd(false)
		, m_solid(false)
		, m_shape(nullptr)
        , m_invalidateMaterial(true)
		, m_invalidateVerticesLocal(true)
		, m_invalidateVerticesWM(true)
		, m_invalidateVerticesColor(true)
		, m_vertexCount(0)
		, m_indicesCount(0)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Mesh2D::~Mesh2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh2D::_compile()
	{
		if( this->compileResource_() == false )
		{
			return false;
		}
      
        this->invalidateMaterial();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh2D::compileResource_()
	{
		if( m_resourceImage == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("Mesh::compileResource_ '%s' image resource null"
                , m_name.c_str() 
                );

            return false;
        }

        if( m_resourceImage.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Mesh::compileResource_ '%s' image resource %s not compile"
                , m_name.c_str() 
                , m_resourceImage->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_release()
	{
        m_resourceImage.release();

		m_material = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    void Mesh2D::setResourceImage( ResourceImage * _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        m_resourceImage = _resourceImage;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImage * Mesh2D::getResourceImage() const
    {        
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Mesh2D::invalidateMaterial()
    {
        m_invalidateMaterial = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::updateMaterial()
	{
        m_invalidateMaterial = false;

		uint32_t texturesNum = 0;
		RenderTextureInterfacePtr textures[2];

		textures[0] = m_resourceImage->getTexture();
		textures[1] = m_resourceImage->getTextureAlpha();
		
        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTextureAlpha();

		ConstString stageName;

        if( textureAlpha != nullptr )
        {
            if( m_resourceImage->isAlpha() == true || m_solid == false )
            {
                texturesNum = 2;

				stageName = CONST_STRING(m_serviceProvider, ExternalAlpha);
            }
            else
            {
                texturesNum = 1;
				
				stageName = CONST_STRING(m_serviceProvider, SolidSprite);
            }
        }
		else if( m_blendAdd == true )
		{
			texturesNum = 1;

			stageName = CONST_STRING(m_serviceProvider, ParticleIntensive);
		}
		else
		{
			texturesNum = 1;

			if( m_resourceImage->isAlpha() == true || m_solid == false )
			{
				stageName = CONST_STRING(m_serviceProvider, BlendSprite);
			}
			else
			{
				stageName = CONST_STRING(m_serviceProvider, SolidSprite);
			}
		}

		bool wrapU = m_resourceImage->isWrapU();
		bool wrapV = m_resourceImage->isWrapV();

		m_material = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( stageName, wrapU, wrapV, PT_TRIANGLELIST, texturesNum, textures );

		if( m_material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Mesh::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );

		if( m_vertexCount == 0 )
		{
			return;
		}

		const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterialInterfacePtr & material = this->getMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject( _viewport, _camera, material, vertices, m_vertexCount, m_shape->indices, m_indicesCount, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const RenderVertex2D * vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( uint32_t i = 1; i != m_vertexCount; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::invalidateVertices()
	{
		m_invalidateVerticesLocal = true;

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;

		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::updateVertices() const
	{
		m_invalidateVerticesLocal = false;

		const mt::vec4f & image_uv = m_resourceImage->getUVImage();
		const mt::vec4f & image_uvAlpha = m_resourceImage->getUVAlpha();
		bool uvRotate = m_resourceImage->isUVRGBRotate();

		if( uvRotate == false )
		{
			// RGB(A)
			{
				for( uint32_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_shape->uv[i];

					float u = image_uv.x + (image_uv.z - image_uv.x) * uv.x;
					float v = image_uv.y + (image_uv.w - image_uv.y) * uv.y;

					m_verticesWM[i].uv.x = u;
					m_verticesWM[i].uv.y = v;
				}
			}

			// Alpha
			//if( m_textures[1] != nullptr )
			{
				for( uint32_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_shape->uv[i];

					float u = image_uvAlpha.x + (image_uvAlpha.z - image_uvAlpha.x) * uv.x;
					float v = image_uvAlpha.y + (image_uvAlpha.w - image_uvAlpha.y) * uv.y;

					m_verticesWM[i].uv2.x = u;
					m_verticesWM[i].uv2.y = v;
				}
			}
		}
		else
		{
			// RGB(A)
			{
				for( uint32_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_shape->uv[i];

					float u = image_uv.z - (image_uv.z - image_uv.x) * uv.y;
					float v = image_uv.y + (image_uv.w - image_uv.y) * uv.x;

					m_verticesWM[i].uv.x = u;
					m_verticesWM[i].uv.y = v;
				}
			}

			// Alpha
			//if( m_textures[1] != nullptr )
			{
				for( uint32_t i = 0; i != m_vertexCount; ++i )
				{
					const mt::vec2f & uv = m_shape->uv[i];

					float u = image_uvAlpha.z - (image_uvAlpha.z - image_uvAlpha.x) * uv.y;
					float v = image_uvAlpha.y + (image_uvAlpha.w - image_uvAlpha.y) * uv.x;

					m_verticesWM[i].uv2.x = u;
					m_verticesWM[i].uv2.y = v;
				}
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Mesh2D::_invalidateColor()
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
	void Mesh2D::invalidateVerticesColor()
	{
		m_invalidateVerticesColor = true;                
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::updateVerticesColor() const
	{
		m_invalidateVerticesColor = false;

		ColourValue color;
		this->calcTotalColor(color);

		uint32_t argb = color.getAsARGB();

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
	void Mesh2D::setBlendAdd( bool _value )
	{
		if( m_blendAdd == _value )
		{
			return;
		}

		m_blendAdd = _value;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh2D::isBlendAdd() const
	{
		return m_blendAdd;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::setFrameShape( const MovieFrameShape * _shape )
	{
		m_shape = _shape;

		m_vertexCount = m_shape->vertexCount;
		m_indicesCount = m_shape->indexCount;

		this->invalidateVertices();
		this->invalidateVerticesColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::updateVerticesWM() const
	{
		m_invalidateVerticesWM = false;

		if( m_invalidateVerticesLocal == true )
		{
			this->updateVertices();
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec2f & pos = m_shape->pos[i];

			mt::vec3f & wm_pos = m_verticesWM[i].pos;
			mt::mul_v3_v2_m4( wm_pos, pos, wm);
		}
	}
}
