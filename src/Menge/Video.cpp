#	include "Video.h"

#	include "ResourceVideo.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/NodeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/VideoCodecInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "pybind/system.hpp"
#	include "Consts.h"

#   include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_surfaceVideo(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setSurfaceVideo( const SurfaceVideoPtr & _surfaceVideo )
	{
		m_surfaceVideo = _surfaceVideo;
	}
	//////////////////////////////////////////////////////////////////////////
	const SurfaceVideoPtr & Video::getSurfaceVideo() const
	{
		return m_surfaceVideo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compile()
	{
		if( m_surfaceVideo == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Video::_compile '%s' can`t setup video surface"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_surfaceVideo->compile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Video::_compile '%s' can`t compile surface video '%s'"
				, this->getName().c_str()
				, m_surfaceVideo->getName().c_str()
				);

			return false;
		}

		this->invalidateVertices();
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		Node::_release();

		m_surfaceVideo->release();        
    }
	//////////////////////////////////////////////////////////////////////////
	void Video::_update( float _current, float _timing )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_surfaceVideo->update( _current, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state )
	{
		const RenderVertex2D * vertices = this->getVertices();

		const RenderMaterialInterfacePtr & material = m_surfaceVideo->getMaterial(); 

		const mt::box2f & bb = this->getBoundingBox();

		_renderService
			->addRenderQuad( _state, material, vertices, 4, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) const
	{
        (void)_invalidateVertices;

		const mt::mat4f & wm = this->getWorldMatrix();

		ColourValue color;
		this->calcTotalColor( color );

		mt::vec3f transformX;
		mt::vec3f transformY;

		float width = m_surfaceVideo->getWidth();
		float height = m_surfaceVideo->getHeight();
		
		mt::mul_v3_v3_m4_r( transformX, mt::vec3f( width, 0.f, 0.f ), wm );
		mt::mul_v3_v3_m4_r( transformY, mt::vec3f( 0.f, height, 0.f ), wm );

		_vertices[0].position = wm.v3.to_vec3f();
		_vertices[1].position = _vertices[0].position + transformX;
		_vertices[2].position = _vertices[1].position + transformY;
		_vertices[3].position = _vertices[0].position + transformY;

		const mt::vec4f & uv = m_surfaceVideo->getUV();

		_vertices[0].uv[0].x = uv.x;
		_vertices[0].uv[0].y = uv.y;
		_vertices[1].uv[0].x = uv.z;
		_vertices[1].uv[0].y = uv.y;
		_vertices[2].uv[0].x = uv.z;
		_vertices[2].uv[0].y = uv.w;
		_vertices[3].uv[0].x = uv.x;
		_vertices[3].uv[0].y = uv.w;

		_vertices[0].uv[1].x = uv.x;
		_vertices[0].uv[1].y = uv.y;
		_vertices[1].uv[1].x = uv.z;
		_vertices[1].uv[1].y = uv.y;
		_vertices[2].uv[1].x = uv.z;
		_vertices[2].uv[1].y = uv.w;
		_vertices[3].uv[1].x = uv.x;
		_vertices[3].uv[1].y = uv.w;

		ColourValue_ARGB argb = color.getAsARGB();

		for( RenderVertex2D
			*it = _vertices,
			*it_end = _vertices + 4;
		it != it_end;
		++it )
		{
			RenderVertex2D & vtx = *it;

			vtx.color = argb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const RenderVertex2D * vertcies = this->getVertices();

		mt::reset( _boundingBox, vertcies[0].position.x, vertcies[0].position.y );

		mt::add_internal_point( _boundingBox, vertcies[1].position.x, vertcies[1].position.y );
		mt::add_internal_point( _boundingBox, vertcies[2].position.x, vertcies[2].position.y );
		mt::add_internal_point( _boundingBox, vertcies[3].position.x, vertcies[3].position.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVertices();
	}
}
