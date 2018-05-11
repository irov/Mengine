#include "MovieSlot.h"

#include "Interface/RenderSystemInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::setMovieName( const ConstString & _movieName )
    {
        m_movieName = _movieName;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_destroy()
    {
#ifndef NDEBUG
        TListNodeChild & child = this->getChildren();

        for( TListNodeChild::iterator
            it = child.begin(),
            it_end = child.end();
        it != it_end;
        ++it )
        {
            NodePtr node = *it;

            LOGGER_ERROR("MovieSlot::_destroy slot %s:%s has children %s:%s!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , m_movieName.c_str()
                );
        }
#endif

		Node::_destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setLocalHide( bool _value )
    {
        Node::_setLocalHide( _value );

        this->setHide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalColor( const ColourValue& _color )
    {
        Node::_setPersonalColor( _color );

        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieSlot::_setPersonalAlpha( float _alpha )
    {
        Node::_setPersonalAlpha( _alpha );

        this->setLocalAlpha( _alpha );
    }
	//////////////////////////////////////////////////////////////////////////
	void MovieSlot::_changeParent( Node * _oldParent, Node * _newParent )
	{
		(void)_newParent;

		if( _oldParent == nullptr )
		{
			return;
		}

		if( _newParent == nullptr )
		{
			return;
		}
		
		LOGGER_ERROR("MovieSlot::_changeParent slot %s:%s movie %s (please don't change parent it's danger)"
			, this->getName().c_str()
			, this->getType().c_str()
			, m_movieName.c_str()
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieSlot::_debugRender( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask )
	{
		if( (_debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		uint32_t numpoints = 4;
		uint32_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = _renderService->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		const mt::mat4f & worldMat = this->getWorldMatrix();

		mt::vec2f ring[4];

		float radius = 10.f;
		float half_radius = radius * 0.5f;
		ring[0] = mt::vec2f( 0, -half_radius );
		ring[1] = mt::vec2f( half_radius, 0 );
		ring[2] = mt::vec2f( 0, half_radius );
		ring[3] = mt::vec2f( -half_radius, 0 );

		for( uint32_t i = 0; i != numpoints; ++i )
		{
			uint32_t j = (i + 1) % numpoints;

			mt::vec3f trP0;
			mt::mul_v3_v2_m4( trP0, ring[i], worldMat );

			RenderVertex2D & v0 = vertices[i * 2 + 0];

			v0.position = trP0;

			v0.color = 0x000000FF;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index)
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;
			}

			mt::vec3f trP1;
			mt::mul_v3_v2_m4( trP1, ring[j], worldMat );

			RenderVertex2D & v1 = vertices[i * 2 + 1];

			v1.position = trP1;

			v1.color = 0x000000FF;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index)
			{
				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
			->getDebugMaterial();

		_renderService->addRenderLine( _state, debugMaterial
			, vertices
			, vertexCount
			, nullptr
			, true
			);
	}
}
