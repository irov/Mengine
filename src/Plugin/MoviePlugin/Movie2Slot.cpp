#include "Movie2Slot.h"
#include "Movie2.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::Movie2Slot()
        : m_movie( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot::~Movie2Slot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::setMovie( const Movie2Ptr & _movie )
    {
        m_movie = _movie;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2Ptr & Movie2Slot::getMovie() const
    {
        return m_movie;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_destroy()
    {
#ifndef NDEBUG
        IntrusiveSlugListNodeChild & child = this->getChildren();

        for( IntrusiveSlugListNodeChild::iterator
            it = child.begin(),
            it_end = child.end();
        it != it_end;
        ++it )
        {
            NodePtr node = *it;

            LOGGER_ERROR("Movie2Slot::_destroy slot %s:%s has children %s:%s!! (please remove, before release movie '%s')"
                , this->getName().c_str()
                , this->getType().c_str()
                , node->getName().c_str()
                , node->getType().c_str()
                , m_movie->getName().c_str()
                );
        }
#endif

		Node::_destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_setLocalHide( bool _value )
    {
        Node::_setLocalHide( _value );

        this->setHide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_setPersonalColor( const ColourValue& _color )
    {
        Node::_setPersonalColor( _color );

        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Slot::_setPersonalAlpha( float _alpha )
    {
        Node::_setPersonalAlpha( _alpha );

        this->setLocalAlpha( _alpha );
    }
	//////////////////////////////////////////////////////////////////////////
	void Movie2Slot::_changeParent( Node * _oldParent, Node * _newParent )
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
			, m_movie->getName().c_str()
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2Slot::_debugRender( const RenderContext * _state )
	{
        if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		uint32_t numpoints = 4;
		uint32_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = this->getDebugRenderVertex2D( vertexCount );

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

			v0.color = 0xFFFFFFFF;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index)
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;
			}

			mt::vec3f trP1;
			mt::mul_v3_v2_m4( trP1, ring[j], worldMat );

			RenderVertex2D & v1 = vertices[i * 2 + 1];

			v1.position = trP1;

			v1.color = 0xFFFFFFFF;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index)
			{
				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

        const RenderMaterialInterfacePtr & debugMaterial = this->getDebugMaterial();

		this->addRenderLine( _state, debugMaterial
			, vertices
			, vertexCount
			, nullptr
			, true
			);
	}
}
