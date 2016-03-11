#	include "Mesh2D.h" 

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Mesh2D::Mesh2D()
		: m_shape( nullptr )
		, m_solid(false)		
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
		Node::_release();

        m_resourceImage.release();

		this->releaseMaterial();
	}
    //////////////////////////////////////////////////////////////////////////
	void Mesh2D::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        m_resourceImage = _resourceImage;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Mesh2D::getResourceImage() const
    {        
        return m_resourceImage;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Mesh2D::_updateMaterial() const
	{
		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_serviceProvider, m_resourceImage, m_solid );
		
		if( material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Mesh::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_render( const RenderObjectState * _state )
	{
		Node::_render( _state );

		if( m_vertexCount == 0 )
		{
			return;
		}

		const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterialInterfacePtr & material = this->getMaterial();

		const mt::box2f & bb = this->getBoundingBox();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject( _state, material, vertices, m_vertexCount, m_shape->indices, m_indicesCount, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh2D::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		if (m_vertexCount == 0)
		{
			mt::reset(_boundingBox, 0.f, 0.f);

			return;
		}

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

		const mt::uv4f & uv_image = m_resourceImage->getUVImage();
		
		for( uint32_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec2f & uv = m_shape->uv[i];

			multiply_tetragon_uv4_v2( m_verticesWM[i].uv[0], uv_image, uv );
		}

		const mt::uv4f & uv_alpha = m_resourceImage->getUVAlpha();

		for( uint32_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec2f & uv = m_shape->uv[i];

			multiply_tetragon_uv4_v2( m_verticesWM[i].uv[1], uv_alpha, uv );
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

		const ColourValue & textureColour = m_resourceImage->getColor();
		color *= textureColour;

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
