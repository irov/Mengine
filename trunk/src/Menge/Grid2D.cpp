#	include "Grid2D.h" 
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Grid2D::Grid2D()
		: m_width(0.f)
		, m_height(0.f)
		, m_countX(0)
		, m_countY(0)
		, m_blendAdd(false)
		, m_invalidateMaterial(true)
		, m_invalidateVerticesWM(true)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Grid2D::~Grid2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::setResourceImage( ResourceImage * _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}

		m_resourceImage = _resourceImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * Grid2D::getResourceImage() const
	{        
		return m_resourceImage;
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

		ARGB argb = _value.getAsARGB();

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
		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Grid2D::compileResource_ '%s' image resource null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceImage.compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Grid2D::compileResource_ '%s' image resource %s not compile"
				, m_name.c_str()
				, m_resourceImage->getName().c_str()
				);

			return false;
		}

		if( m_countX < 2 || m_countY < 2 )
		{
			LOGGER_ERROR(m_serviceProvider)("Grid2D::compileResources_ '%s' count X|Y not setup %d:%d"
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

				vertex.pos.x = pos_step_x * i;
				vertex.pos.y = pos_step_y * j;
				vertex.pos.z = 0.f;

				vertex.color = 0xFFFFFFFF;

				vertex.uv.x = pos_uv_x * i;
				vertex.uv.y = pos_uv_y * j;
				vertex.uv2.x = pos_uv_x * i;
				vertex.uv2.y = pos_uv_y * j;
			}
		}

		m_indices.resize( (m_countX - 1) * (m_countY - 1) * 6 );

		TVectorRenderIndices2D::iterator indices_iterator = m_indices.begin();
		
		for( uint32_t j = 0; j != (m_countY - 1); ++j )
		{
			for( uint32_t i = 0; i != (m_countX - 1); ++i )
			{
				uint32_t i0 = (i + 0) + (j + 0) * m_countX;
				uint32_t i1 = (i + 1) + (j + 0) * m_countX;
				uint32_t i2 = (i + 0) + (j + 1) * m_countX;
				uint32_t i3 = (i + 1) + (j + 1) * m_countX;

				*indices_iterator++ = (uint16_t)i0;
				*indices_iterator++ = (uint16_t)i2;
				*indices_iterator++ = (uint16_t)i1;
				*indices_iterator++ = (uint16_t)i1;
				*indices_iterator++ = (uint16_t)i2;
				*indices_iterator++ = (uint16_t)i3;
			}
		}

		m_invalidateVerticesWM = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_release()
	{
		m_resourceImage.release();
				
		m_vertices.clear();
		m_verticesWM.clear();
		m_indices.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::updateMaterial_()
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
			if( m_resourceImage->isAlpha() == true )
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

			if( m_resourceImage->isAlpha() == true )
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
			LOGGER_ERROR(m_serviceProvider)("Grid2D::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );

		const RenderIndices2D * indices = &m_indices[0];
		size_t indicesCount = m_indices.size();
		
		const RenderVertex2D * vertices = this->getVerticesWM();
		const RenderMaterialInterfacePtr & material = this->getMaterial();

		size_t verticesCount = m_verticesWM.size();
		
		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject( _viewport, _camera, material, vertices, verticesCount, indices, indicesCount, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		TVectorRenderVertex2D::const_iterator it = m_vertices.begin();
		TVectorRenderVertex2D::const_iterator it_end = m_vertices.end();

		TVectorRenderVertex2D::iterator it_w = m_verticesWM.begin();

		const mt::mat4f & wm = this->getWorldMatrix();
		
		for( ; it != it_end; ++it, ++it_w )
		{
			const RenderVertex2D & vertex = *it;

			RenderVertex2D & vertex_w = *it_w;

			mt::mul_v3_m4( vertex_w.pos, vertex.pos, wm );
			
			vertex_w.color = vertex.color;
			vertex_w.uv = vertex.uv;
			vertex_w.uv2 = vertex.uv2;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Grid2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		m_invalidateVerticesWM = true;
	}
}
