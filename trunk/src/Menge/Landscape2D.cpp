#	include "Landscape2D.h" 
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
	Landscape2D::Landscape2D()
		: m_countX(0)
		, m_countY(0)
		, m_invalidateVerticesWM(true)
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

		if( m_countX == 0 || m_countY == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' count X|Y not setup %d:%d"
				, m_name.c_str() 
				, m_countX
				, m_countY
				);

			return false;
		}

		size_t imageSize = m_images.size();
		m_elements.reserve( imageSize );

		for( TVectorResourceImage::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			ResourceImage * image = *it;

			if( image == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' images resource not setup"
					, m_name.c_str() 
					);

				return false;
			}

			Element el;
			el.resource = image;

			if( el.resource->compile() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' image resource %s not compile"
					, m_name.c_str() 
					, el.resource->getName().c_str()
					);

				return false;
			}

			const RenderTextureInterfacePtr & texture = el.resource->getTexture();

			el.material = RENDERMATERIAL_SERVICE(m_serviceProvider)
				->getMaterial( CONST_STRING(m_serviceProvider, SolidSprite), false, false, PT_TRIANGLELIST, 1, &texture );

			if( el.material == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Landscape2D::compileResources_ '%s' invalid get material"
					, m_name.c_str() 
					);

				return false;
			}

			m_elements.push_back( el );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_release()
	{
		for( TVectorElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Element & el = *it;
			
			el.resource->release();
			el.material = nullptr;
		}

		m_verticesWM.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );
		
		const RenderVertex2D * vertices = this->getVerticesWM();

		size_t verticesOffset = 0;

		for( TVectorElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Element & el = *it;

			RENDER_SERVICE(m_serviceProvider)
				->addRenderQuad( _viewport, _camera, el.material, vertices + verticesOffset, 4, nullptr );

			verticesOffset += 4;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::setBackParts( const TVectorResourceImage & _images, size_t _countX, size_t _countY)
	{
		m_countX = _countX;
		m_countY = _countY;

		m_images = _images;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateVerticesWM_()
	{
		m_invalidateVerticesWM = false;

		size_t length = m_elements.size() * 4;
		m_verticesWM.resize( length );

		const mt::mat4f & wm = this->getWorldMatrix();
		
		float x_offset = 0.f;
		float y_offset = 0.f;
		size_t index_offset = 0;

		size_t vertexOffset = 0;

		for( TVectorElements::iterator
			it = m_elements.begin(),
			it_end = m_elements.end();
		it != it_end;
		++it )
		{
			Element & el = *it;

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

			m_verticesWM[vertexOffset + 0] = v1;

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

			m_verticesWM[vertexOffset + 1] = v2;

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

			m_verticesWM[vertexOffset + 2] = v3;

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

			m_verticesWM[vertexOffset + 3] = v4;

			vertexOffset += 4;

			x_offset += sz.x;

			++index_offset;
			if( (index_offset % m_countX) == 0 )
			{
				x_offset = 0.f;
				y_offset += sz.y;
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
}
