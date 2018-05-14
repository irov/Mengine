#include "Meshget.h" 

#include "Interface/RenderSystemInterface.h"

#include "Kernel/ResourceImage.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Meshget::Meshget()
		: m_solid( false )
		, m_invalidateVerticesWM( true )
		, m_invalidateVerticesColor( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Meshget::~Meshget()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Meshget::_compile()
	{
		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR("Meshget::_compile '%s' image resource null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceImage.compile() == false )
		{
			LOGGER_ERROR("Meshget::_compile '%s' image resource %s not compile"
				, m_name.c_str()
				, m_resourceImage->getName().c_str()
				);

			return false;
		}

		this->invalidateMaterial();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_release()
	{
		Node::_release();

		m_resourceImage.release();

		this->releaseMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::setResourceImage( const ResourceImagePtr & _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}

		m_resourceImage = _resourceImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & Meshget::getResourceImage() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Meshget::setVertices( const pybind::list & _positions, const pybind::list & _uv, const pybind::list & _colors, const pybind::list & _indices )
	{
		size_t positions_count = _positions.size();
		size_t uvs_count = _uv.size();
		size_t colors_count = _colors.size();
		
		if( positions_count != uvs_count || positions_count != colors_count )
		{
			return false;
		}

		m_positions.resize( positions_count );
		m_uvs.resize( uvs_count );
		m_colors.resize( colors_count );

		for( uint32_t i = 0; i != positions_count; ++i )
		{
			m_positions[i] = _positions[i];
			m_uvs[i] = _uv[i];
			m_colors[i] = _colors[i];
		}

		size_t indices_count = _indices.size();

		m_indices.resize( indices_count );

		for( uint32_t i = 0; i != indices_count; ++i )
		{
			m_indices[i] = _indices[i];
		}

		m_verticesWM.resize( positions_count );

		this->invalidateVerticesColor();
		this->invalidateVerticesWM();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Meshget::_updateMaterial() const
	{
		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, m_solid );

		if( material == nullptr )
		{
			LOGGER_ERROR("Mesh::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_update( float _current, float _timing )
	{
        EVENTABLE_METHOD( this, EVENT_MESHGET_UPDATE )
            ->onMeshgetUpdate( _current, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_render( RenderServiceInterface * _renderService, const RenderState * _state )
	{
		if( m_positions.empty() == true )
		{
			return;
		}

		uint32_t vertexCount = (uint32_t)m_positions.size();
		uint32_t indicesCount = (uint32_t)m_indices.size();

		const TVectorRenderVertex2D & vertices = this->getVerticesWM();
		const RenderMaterialInterfacePtr & material = this->getMaterial();

		const RenderVertex2D * vertices_buff = &vertices[0];
		const RenderIndex * indices_buff = &m_indices[0];

		const mt::box2f & bb = this->getBoundingBox();

		_renderService
			->addRenderObject( _state, material, vertices_buff, vertexCount, indices_buff, indicesCount, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		if( m_positions.empty() == true )
		{
			mt::ident_box( _boundingBox );

			return;
		}

		const TVectorRenderVertex2D & vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

		size_t vertexCount = m_positions.size();
				
		for( uint32_t i = 1; i != vertexCount; ++i )
		{
			const mt::vec3f & v = m_positions[i];

			mt::add_internal_point( _boundingBox, v.x, v.y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;

		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::_invalidateColor()
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
	void Meshget::invalidateVerticesColor()
	{
		m_invalidateVerticesColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::updateVerticesColor() const
	{
		m_invalidateVerticesColor = false;

		ColourValue color;
		this->calcTotalColor( color );

		const ColourValue & textureColour = m_resourceImage->getColor();
		color *= textureColour;

		size_t vertexCount = m_positions.size();

		for( uint32_t i = 0; i != vertexCount; ++i )
		{
			const mt::vec4f & c = m_colors[i];

			ColourValue v_color( c.x, c.y, c.z, c.w );
			v_color *= color;
			
			uint32_t argb = v_color.getAsARGB();

			m_verticesWM[i].color = argb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Meshget::updateVerticesWM() const
	{
		m_invalidateVerticesWM = false;

		size_t vertexCount = m_positions.size();

		const mt::mat4f & wm = this->getWorldMatrix();
		
		for( uint32_t i = 0; i != vertexCount; ++i )
		{
			const mt::vec3f & pos = m_positions[i];
			const mt::vec2f & uv = m_uvs[i];

			mt::vec3f & wm_pos = m_verticesWM[i].position;
			mt::mul_v3_v3_m4( wm_pos, pos, wm );

            m_resourceImage->correctUVImage( m_verticesWM[i].uv[0], uv );
            m_resourceImage->correctUVAlpha( m_verticesWM[i].uv[1], uv );
		}
	}
}
