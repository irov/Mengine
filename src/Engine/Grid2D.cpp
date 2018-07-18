#include "Grid2D.h" 
#include "Layer2D.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceInterface.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"

#include "math/box2.h"
#include "math/clamp.h"

#include <math.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Grid2D::Grid2D()
		: m_width( 0.f )
		, m_height( 0.f )
		, m_countX( 0 )
		, m_countY( 0 )
		, m_invalidateVerticesWM( true )
		, m_offset(0.f, 0.f)
		, m_angle(0.f)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Grid2D::~Grid2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setResourceImage( const ResourceImagePtr & _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}
        
        this->recompile( [this, _resourceImage]() {m_resourceImage = _resourceImage; } );
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & Grid2D::getResourceImage() const
	{        
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setAngle( const mt::vec2f & _offset, float _angle )
	{ 
		m_offset = _offset;
		m_angle = _angle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setWidth( float _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	float Grid2D::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setHeight( float _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	float Grid2D::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setCountX( uint32_t _count )
	{
		m_countX = _count;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Grid2D::getCountX() const
	{
		return m_countX;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setCountY( uint32_t _count )
	{
		m_countY = _count;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Grid2D::getCountY() const
	{
		return m_countY;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Grid2D::setGridColor( uint32_t _i, uint32_t _j, const ColourValue & _value )
	{
		if( _i >= m_countX || _j >= m_countY )
		{
			return false;
		}

		uint32_t index = _i + _j * m_countX;

		ColourValue_ARGB argb = _value.getAsARGB();

		m_vertices[index].color = argb;

		m_invalidateVerticesWM = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Grid2D::getGridColor( uint32_t _i, uint32_t _j, ColourValue & _value ) const
	{
		if( _i >= m_countX || _j >= m_countY )
		{
			return false;
		}

		uint32_t index = _i + _j * m_countX;

		_value.setAsARGB( m_vertices[index].color );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Grid2D::_compile()
	{
		if( m_resourceImage != nullptr )
		{
			if( m_resourceImage.compile() == false )
			{
				LOGGER_ERROR("Grid2D::compileResource_ '%s' image resource %s not compile"
					, m_name.c_str()
					, m_resourceImage->getName().c_str()
					);

				return false;
			}
		}

		if( m_countX < 2 || m_countY < 2 )
		{
			LOGGER_ERROR("Grid2D::compileResources_ '%s' count X|Y not setup %d:%d"
				, m_name.c_str() 
				, m_countX
				, m_countY
				);

			return false;
		}

		m_vertices.resize( m_countX * m_countY );
		m_verticesWM.resize( m_countX * m_countY );

		TVectorRenderVertex2D::iterator vertices_iterator = m_vertices.begin();

		float pos_step_x = m_width / float(m_countX - 1);
		float pos_step_y = m_height / float(m_countY - 1);

		float pos_uv_x = 1.f / float(m_countX - 1);
		float pos_uv_y = 1.f / float(m_countY - 1);

		for( uint32_t j = 0; j != m_countY; ++j )
		{
			for( uint32_t i = 0; i != m_countX; ++i )
			{
				RenderVertex2D & vertex = *vertices_iterator++;

				vertex.position.x = pos_step_x * i;
				vertex.position.y = pos_step_y * j;
				vertex.position.z = 0.f;

				vertex.color = 0xFFFFFFFF;

				vertex.uv[0].x = m_offset.x + pos_uv_x * i;
				vertex.uv[0].y = m_offset.y + pos_uv_y * j;
				vertex.uv[1].x = m_offset.x + pos_uv_x * i;
				vertex.uv[1].y = m_offset.y + pos_uv_y * j;
			}
		}

		m_indices.resize( (m_countX - 1) * (m_countY - 1) * 6 );

		TVectorRenderIndices::iterator indices_iterator = m_indices.begin();
		
		for( uint32_t j = 0; j != (m_countY - 1); ++j )
		{
			for( uint32_t i = 0; i != (m_countX - 1); ++i )
			{
				uint32_t i0 = (i + 0) + (j + 0) * m_countX;
				uint32_t i1 = (i + 1) + (j + 0) * m_countX;
				uint32_t i2 = (i + 0) + (j + 1) * m_countX;
				uint32_t i3 = (i + 1) + (j + 1) * m_countX;

				*indices_iterator++ = (RenderIndex)i0;
				*indices_iterator++ = (RenderIndex)i2;
				*indices_iterator++ = (RenderIndex)i1;
				*indices_iterator++ = (RenderIndex)i1;
				*indices_iterator++ = (RenderIndex)i2;
				*indices_iterator++ = (RenderIndex)i3;
			}
		}

		m_invalidateVerticesWM = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_release()
	{
		Node::_release();

		m_resourceImage.release();
				
		m_vertices.clear();
		m_verticesWM.clear();
		m_indices.clear();

		this->releaseMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Grid2D::_updateMaterial() const
	{
		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false );

		if( material == nullptr )
		{
			LOGGER_ERROR("Grid2D::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_update( float _current, float _timing )
	{
		(void)_current;

		for( RenderVertex2D & v : m_vertices )
		{
			v.uv[0].x += 0.00001f * _timing * cosf( m_angle );
			v.uv[0].y += 0.00001f * _timing * sinf( m_angle );
			v.uv[1].x += 0.00001f * _timing * cosf( m_angle );
			v.uv[1].y += 0.00001f * _timing * sinf( m_angle );
		}

		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_render( const RenderContext * _state )
	{
		const RenderIndex * indices = &m_indices[0];
		uint32_t indicesCount = (uint32_t)m_indices.size();
		
		const RenderVertex2D * vertices = this->getVerticesWM();
		const RenderMaterialInterfacePtr & material = this->getMaterial();

		uint32_t verticesCount = (uint32_t)m_verticesWM.size();

		const mt::box2f & bb = this->getBoundingBox();
		
        this->addRenderObject( _state, material, vertices, verticesCount, indices, indicesCount, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		TVectorRenderVertex2D::const_iterator it = m_vertices.begin();
		TVectorRenderVertex2D::const_iterator it_end = m_vertices.end();

		TVectorRenderVertex2D::iterator it_w = m_verticesWM.begin();

		ColourValue color;
		this->calcTotalColor(color);

		if( m_resourceImage != nullptr )
		{
			const ColourValue & textureColour = m_resourceImage->getColor();
			color *= textureColour;
		}

		const mt::mat4f & wm = this->getWorldMatrix();
		
		for( ; it != it_end; ++it, ++it_w )
		{
			const RenderVertex2D & vertex = *it;

			RenderVertex2D & vertex_w = *it_w;

			mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );

			ColourValue vertex_color(color);
			vertex_color *= ColourValue(vertex.color);

			uint32_t argb = vertex_color.getAsARGB();
			
			vertex_w.color = argb;
			vertex_w.uv[0] = vertex.uv[0];
			vertex_w.uv[1] = vertex.uv[1];
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec2f minimal( 0.f, 0.f );
		mt::vec2f maximal( m_width, m_height );

		mt::vec2f minimal_wm;
		mt::mul_v2_v2_m4( minimal_wm, minimal, wm );

		mt::vec2f maximal_wm;
		mt::mul_v2_v2_m4( maximal_wm, maximal, wm );

		mt::set_box_from_two_point( _boundingBox, minimal_wm, maximal_wm );
	}
}
