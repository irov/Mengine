#	include "Landscape2D.h" 
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
	Landscape2D::Landscape2D()
		: m_elementCountX(0)
		, m_elementCountY(0)
		, m_invalidateVerticesWM(true)
		, m_invalidateElementWM(true)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Landscape2D::~Landscape2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Landscape2D::_compile()
	{
		if( m_images.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' images is empty"
				, m_name.c_str() 
				);

			return false;
		}

		if( m_elementCountX == 0 || m_elementCountY == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' count X|Y not setup %d:%d"
				, m_name.c_str() 
				, m_elementCountX
				, m_elementCountY
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_release()
	{
		for( TVectorLandscape2DElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Landscape2DElement & el = *it;
			
			el.resource->decrementReference();
			el.resource = nullptr;

			el.material = nullptr;
		}

		m_elements.clear();

		m_verticesWM.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );
		
		const mt::mat4f & wm = this->getWorldMatrix();

		const mt::box2f & cameraBBWM = _camera->getCameraBBoxWM();

		TVectorLandscape2DElements & elementsWM = this->getElementWM();

		for( TVectorLandscape2DElements::iterator
			it = elementsWM.begin(),
			it_end = elementsWM.end();
		it != it_end;
		++it )
		{
			Landscape2DElement & el = *it;

			if( mt::is_intersect( el.bb_wm, cameraBBWM ) == false )
			{
				if( el.refcount == 0 )
				{
					continue;
				}

				--el.refcount;

				if( el.refcount == 0 )
				{
					el.material = nullptr;
				}
			}
			else
			{
				++el.refcount;

				if( el.refcount == 1 )
				{
					if( el.resource->incrementReference() == 0 )
					{
						LOGGER_ERROR(m_serviceProvider)("Landscape2D::_render '%s' image resource %s not compile"
							, m_name.c_str() 
							, el.resource->getName().c_str()
							);

						return;
					}

					const RenderTextureInterfacePtr & texture = el.resource->getTexture();

					el.material = RENDERMATERIAL_SERVICE(m_serviceProvider)
						->getMaterial( CONST_STRING(m_serviceProvider, SolidSprite), false, false, PT_TRIANGLELIST, 1, &texture );

					if( el.material == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("Landscape2D::_render '%s' invalid get material"
							, m_name.c_str() 
							);

						return;
					}
				}
			}
		}
		
		uint32_t elementVertexOffset = 0;

		for( TVectorLandscape2DElements::const_iterator
			it = elementsWM.begin(),
			it_end = elementsWM.end();
		it != it_end;
		++it )
		{
			const Landscape2DElement & el = *it;

			if( el.refcount == 0 )
			{
				continue;
			}
			
			RenderVertex2D * v = &m_verticesWM[elementVertexOffset];
			
			this->updateElementVertex_( el, wm, v );

			RENDER_SERVICE(m_serviceProvider)
				->addRenderQuad( _viewport, _camera, el.material, v, 4, nullptr );

			elementVertexOffset += 4;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::setBackParts( const TVectorResourceImage & _images, uint32_t _countX, uint32_t _countY, float _width, float _height)
	{
		m_elementCountX = _countX;
		m_elementCountY = _countY;
		m_elementWidth = _width;
		m_elementHeight = _height;

		m_images = _images;

		uint32_t i = 0;
		uint32_t j = 0;

		uint32_t elementCount = m_images.size();
		m_elements.reserve( elementCount );
		m_verticesWM.resize( elementCount * 4 );

		for( TVectorResourceImage::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			ResourceImage * image = *it;

			Landscape2DElement el;

			el.resource = image;
			el.refcount = 0;

			// left-upper
			mt::vec2f p1;
			p1.x = i * m_elementWidth;
			p1.y = j * m_elementHeight;

			mt::vec2f p2;
			p2.x = i * m_elementWidth + m_elementWidth;
			p2.y = j * m_elementHeight;

			mt::vec2f p3;
			p3.x = i * m_elementWidth + m_elementWidth;
			p3.y = j * m_elementHeight + m_elementHeight;

			mt::vec2f p4;
			p4.x = i * m_elementWidth;
			p4.y = j * m_elementHeight + m_elementHeight;

			mt::reset( el.bb, p1 );
			mt::add_internal_point( el.bb, p2 );
			mt::add_internal_point( el.bb, p3 );
			mt::add_internal_point( el.bb, p4 );

			el.i = i;
			el.j = j;

			++i;

			if( i == m_elementCountX )
			{
				i = 0;
				++j;
			}

			m_elements.push_back( el );
		}	

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateElementVertex_( const Landscape2DElement & _el, const mt::mat4f & _wm, RenderVertex2D * _vertices )
	{
		const mt::vec4f & uv = _el.resource->getUVImage();

		// left-upper
		RenderVertex2D v1;
		v1.color = 0xffffffff;
		v1.uv.x = uv.x;
		v1.uv.y = uv.y;
		v1.uv2.x = 0.f;
		v1.uv2.y = 0.f;

		mt::vec3f p1;
		p1.x = _el.i * m_elementWidth;
		p1.y = _el.j * m_elementHeight;
		p1.z = 0.f;
		mt::mul_v3_m4( v1.pos, p1, _wm );

		_vertices[0] = v1;

		// right-upper
		RenderVertex2D v2;
		v2.color = 0xffffffff;
		v2.uv.x = uv.z;
		v2.uv.y = uv.y;
		v2.uv2.x = 0.f;
		v2.uv2.y = 0.f;

		mt::vec3f p2;
		p2.x = _el.i * m_elementWidth + m_elementWidth;
		p2.y = _el.j * m_elementHeight;
		p2.z = 0.f;
		mt::mul_v3_m4( v2.pos, p2, _wm );

		_vertices[1] = v2;

		// right-down
		RenderVertex2D v3;
		v3.color = 0xffffffff;
		v3.uv.x = uv.z;
		v3.uv.y = uv.w;
		v3.uv2.x = 0.f;
		v3.uv2.y = 0.f;

		mt::vec3f p3;
		p3.x = _el.i * m_elementWidth + m_elementWidth;
		p3.y = _el.j * m_elementHeight + m_elementHeight;
		p3.z = 0.f;
		mt::mul_v3_m4( v3.pos, p3, _wm );

		_vertices[2] = v3;

		// left-down
		RenderVertex2D v4;
		v4.color = 0xffffffff;
		v4.uv.x = uv.x;
		v4.uv.y = uv.w;
		v4.uv2.x = 0.f;
		v4.uv2.y = 0.f;

		mt::vec3f p4;
		p4.x = _el.i * m_elementWidth;
		p4.y = _el.j * m_elementHeight + m_elementHeight;
		p4.z = 0.f;
		mt::mul_v3_m4( v4.pos, p4, _wm );

		_vertices[3] = v4;
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();
		
		float x_offset = 0.f;
		float y_offset = 0.f;

		uint32_t index_offset = 0;
		uint32_t vertex_offset = 0;

		for( TVectorLandscape2DElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Landscape2DElement & el = *it;

			const mt::vec4f & uv = el.resource->getUVImage();
			const mt::vec2f & sz = el.resource->getMaxSize();

			// left-upper
			RenderVertex2D v1;
			v1.color = 0xffffffff;
			v1.uv.x = uv.x;
			v1.uv.y = uv.y;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;

			mt::vec3f p1;
			p1.x = x_offset;
			p1.y = y_offset;
			p1.z = 0.f;
			mt::mul_v3_m4( v1.pos, p1, wm );

			m_verticesWM[vertex_offset + 0] = v1;

			// right-upper
			RenderVertex2D v2;
			v2.color = 0xffffffff;
			v2.uv.x = uv.z;
			v2.uv.y = uv.y;
			v2.uv2.x = 0.f;
			v2.uv2.y = 0.f;

			mt::vec3f p2;
			p2.x = x_offset + sz.x;
			p2.y = y_offset;
			p2.z = 0.f;
			mt::mul_v3_m4( v2.pos, p2, wm );

			m_verticesWM[vertex_offset + 1] = v2;

			// right-down
			RenderVertex2D v3;
			v3.color = 0xffffffff;
			v3.uv.x = uv.z;
			v3.uv.y = uv.w;
			v3.uv2.x = 0.f;
			v3.uv2.y = 0.f;

			mt::vec3f p3;
			p3.x = x_offset + sz.x;
			p3.y = y_offset + sz.y;
			p3.z = 0.f;
			mt::mul_v3_m4( v3.pos, p3, wm );

			m_verticesWM[vertex_offset + 2] = v3;

			// left-down
			RenderVertex2D v4;
			v4.color = 0xffffffff;
			v4.uv.x = uv.x;
			v4.uv.y = uv.w;
			v4.uv2.x = 0.f;
			v4.uv2.y = 0.f;

			mt::vec3f p4;
			p4.x = x_offset;
			p4.y = y_offset + sz.y;
			p4.z = 0.f;
			mt::mul_v3_m4( v4.pos, p4, wm );

			m_verticesWM[vertex_offset + 3] = v4;
			
			vertex_offset += 4;

			x_offset += sz.x;

			++index_offset;
			if( (index_offset % m_elementCountX) == 0 )
			{
				x_offset = 0.f;
				y_offset += sz.y;
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateElementWM_()
	{
		m_invalidateElementWM = true;

		const mt::mat4f & wm = this->getWorldMatrix();

		for( TVectorLandscape2DElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Landscape2DElement & el = *it;

			mt::mul_v2_m4( el.bb_wm.minimum, el.bb.minimum, wm );
			mt::mul_v2_m4( el.bb_wm.maximum, el.bb.maximum, wm );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		m_invalidateVerticesWM = true;
		m_invalidateElementWM = true;
	}
}
