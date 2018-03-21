#include "NodeMagnetActor.h"

#include "Interface/RenderSystemInterface.h"

#include <math.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	NodeMagnetActor::NodeMagnetActor()
		: m_magnetNode( nullptr )
		, m_magnetRadius( 0.f )
		, m_magnetForce( 0.f )
		, m_magnetActive( false )
		, m_iff( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	NodeMagnetActor::~NodeMagnetActor()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetRadius( float _magnetRadius )
	{
		m_magnetRadius = _magnetRadius;

		if( m_actor != nullptr )
		{
			m_actor->setRadius( m_magnetRadius );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float NodeMagnetActor::getMagnetRadius() const
	{
		return m_magnetRadius;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetForce( float _magnetForce )
	{
		m_magnetForce = _magnetForce;

		if( m_actor != nullptr )
		{
			m_actor->setForce( m_magnetForce );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float NodeMagnetActor::getMagnetForce() const
	{
		return m_magnetForce;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetActive( bool _magnetActive )
	{
		m_magnetActive = _magnetActive;

		if( m_actor != nullptr )
		{
			m_actor->setActive( m_magnetActive );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeMagnetActor::isMagnetActive() const
	{
		return m_magnetActive;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetIFF( uint32_t _collisionIFF )
	{
		m_iff = _collisionIFF;

		if( m_actor != nullptr )
		{
			m_actor->setIFF( m_iff );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t NodeMagnetActor::getMagnetIFF() const
	{
		return m_iff;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetNodeAffect( Node * _magnetNode )
	{
		m_magnetNode = _magnetNode;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::addMagnetException( NodeMagnetActor * _actor )
	{
		if( _actor->isActivate() == false )
		{
			return;
		}

		const MagnetActorPtr & exception_actor = _actor->m_actor;

		m_exceptions.push_back( exception_actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::setMagnetWorld( const MagnetWorldPtr & _magnet )
	{
		m_magnetWorld = _magnet;
	}
	//////////////////////////////////////////////////////////////////////////
	const MagnetWorldPtr & NodeMagnetActor::getMagnetWorld() const
	{
		return m_magnetWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::onMagnetPositionProvider( mt::vec3f & _position ) const
	{
		const mt::vec3f & wp = this->getWorldPosition();

		_position = wp;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::onMagnetAffect( const mt::vec3f & _velocity )
	{
		if( m_magnetNode == nullptr )
		{
			this->translate( _velocity );
		}
		else
		{
			m_magnetNode->translate( _velocity );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeMagnetActor::_activate()
	{
		if( m_magnetWorld == nullptr )
		{
			return false;
		}

		const MagnetActorPtr & actor = m_magnetWorld->createActor( this, m_magnetRadius, m_magnetForce, m_iff, m_magnetActive );

		for( TVectorActorException::iterator
			it = m_exceptions.begin(),
			it_end = m_exceptions.end();
		it != it_end;
		++it )
		{
			const MagnetActorPtr & exception = *it;

			actor->addException( exception );
		}

		actor->setActive( m_magnetActive );
		actor->setRadius( m_magnetRadius );
		actor->setForce( m_magnetForce );
		
		m_exceptions.clear();

		m_actor = actor;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::_deactivate()
	{
		m_magnetWorld->removeActor( m_actor );

		m_actor = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeMagnetActor::_debugRender( Mengine::RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask )
	{
		(void)_debugMask;

		mt::vec2f v[16 + 1];

		for( uint32_t i = 0; i != 16; ++i )
		{
			v[i].x = m_magnetRadius * ::cosf( mt::constant::two_pi / 16.f * i );
			v[i].y = m_magnetRadius * ::sinf( mt::constant::two_pi / 16.f * i );
		}

		v[16] = v[0];

		RenderVertex2D * vertices = _renderService
			->getDebugRenderVertex2D( 17 * 2 );

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t i = 0; i != 16; ++i )
		{
			size_t j = (i + 1) % 16;

			RenderVertex2D & v0 = vertices[i * 2 + 0];
			RenderVertex2D & v1 = vertices[i * 2 + 1];

			mt::mul_v3_v2_m4( v0.position, v[i], wm );
			mt::mul_v3_v2_m4( v1.position, v[j], wm );

			v0.color = 0x8080FFFF;
			v1.color = 0x8080FFFF;
			
			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;

				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
			->getDebugMaterial();

		_renderService->addRenderLine( _state, debugMaterial
			, vertices
			, 17 * 2
			, nullptr
			, true
			);
	}
}