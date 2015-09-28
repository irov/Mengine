#	include "Landscape2D.h" 
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/utils.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Landscape2D::Landscape2D()
		: m_elementCountX(0)
		, m_elementCountY(0)
		, m_elementWidth( 0.f )
		, m_elementHeight( 0.f )
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
			
			el.image->decrementReference();
			el.image = nullptr;

			el.material = nullptr;
		}

		m_elements.clear();

		m_verticesWM.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane )
	{
		Node::_render( _viewport, _camera, _clipplane );
		
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
				if( el.material != nullptr )
				{
					el.image->decrementReference();

					el.material = nullptr;
				}
			}
			else
			{
				if( el.material == nullptr )
				{
					if( el.image->incrementReference() == false )
					{
						LOGGER_ERROR(m_serviceProvider)("Landscape2D::_render '%s' image resource %s not compile"
							, m_name.c_str() 
							, el.image->getName().c_str()
							);

						return;
					}

					RenderMaterialInterfacePtr material = this->makeImageMaterial( m_serviceProvider, el.image, false );

					if( material == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("Landscape2D::_render '%s' invalid get material"
							, m_name.c_str() 
							);

						return;
					}

					el.material = material;
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

			if( el.material != nullptr )
			{
				const RenderVertex2D * vertices = this->getVerticesWM( elementVertexOffset );

				RENDER_SERVICE( m_serviceProvider )
					->addRenderQuad( _viewport, _camera, _clipplane, el.material, vertices, 4, nullptr, false );
			}

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

			if( image == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Landscape2D::setBackParts %s invalid setup image for %d:%d"
					, this->getName().c_str()
					, i
					, j
					);

				continue;
			}

			Landscape2DElement el;

			el.image = image;

			// left-upper
			mt::vec2f p1;
			p1.x = (i + 0) * m_elementWidth;
			p1.y = (j + 0) * m_elementHeight;

			mt::vec2f p2;
			p2.x = (i + 1) * m_elementWidth;
			p2.y = (j + 0) * m_elementHeight;

			mt::vec2f p3;
			p3.x = (i + 1) * m_elementWidth;
			p3.y = (j + 1) * m_elementHeight;

			mt::vec2f p4;
			p4.x = (i + 0) * m_elementWidth;
			p4.y = (j + 1) * m_elementHeight;

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
	void Landscape2D::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		const mt::mat4f & wm = this->getWorldMatrix();
		
		const uint32_t coeff_x[4] = {0, 1, 1, 0};
		const uint32_t coeff_y[4] = {0, 0, 1, 1};

		uint32_t vertex_offset = 0;

		for( TVectorLandscape2DElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Landscape2DElement & el = *it;

			const mt::uv4f & uv_image = el.image->getUVImage();
			const mt::uv4f & uv_alpha = el.image->getUVAlpha();

			for( size_t i = 0; i != 4; ++i )
			{
				// left-upper
				RenderVertex2D & v = m_verticesWM[vertex_offset + i];
				
				v.color = 0xffffffff;
				v.uv[0] = uv_image[i];
				v.uv[1] = uv_alpha[i];

				mt::vec3f p;
				p.x = (el.i + coeff_x[i]) * m_elementWidth;
				p.y = (el.j + coeff_y[i]) * m_elementHeight;
				p.z = 0.f;

				mt::mul_v3_m4( v.pos, p, wm );
			}
			
			vertex_offset += 4;
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateElementWM_()
	{
		m_invalidateElementWM = false;

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
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Landscape2D::_updateMaterial() const
	{
		return nullptr;
	}
}
