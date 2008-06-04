#	include "Emitter.h" 

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "RenderEngine.h"

#	include "Application.h"

#	include "ParticleEngine.h"

#	include "ResourceEmitterContainer.h"

#	include "ResourceManager.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Emitter);
	//////////////////////////////////////////////////////////////////////////
	Emitter::Emitter()
		: m_interface( 0 )
		, m_resource( 0 )
		, m_blendSrc( BF_SOURCE_ALPHA )
		, m_blendDest( BF_ONE_MINUS_SOURCE_ALPHA )
	{}
	//////////////////////////////////////////////////////////////////////////
	Emitter::~Emitter()
	{}	
	///////////////////////////////////////////////////////////////////////////
	bool Emitter::_checkVisibility( const Viewport & _viewPort )
	{
		int left;
		int top; 
		int right;
		int bottom;

		m_interface->getBoundingBox( left, top, right, bottom );

		const mt::mat3f & wm = getWorldMatrix();

		mt::box2f bbox;	//������� box2i

		mt::mul_v2_m3( bbox.min, mt::vec2f( float(left), float(top) ), wm );
		mt::mul_v2_m3( bbox.max, mt::vec2f( float(right), float(bottom) ), wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::_activate()
	{
		bool enabled = Holder<Application>::hostage()->getParticlesEnabled();
		if( !enabled || Node::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::loader( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Resource", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "Emitter", "Name", m_emitterName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceEmitterContainer>( m_resourcename );

		if( m_resource == NULL )
		{
			MENGE_LOG( "Error: Emitter can't open resource file '%s'\n", m_resourcename.c_str() );
			return false;
		}

		const EmitterContainerInterface * m_container = m_resource->getContainer();

		if( m_container == NULL )
		{
			MENGE_LOG( "Error: Emitter can't open container file '%s'\n", m_resourcename.c_str() );
			return false;
		}

		m_interface = Holder<ParticleEngine>::hostage()->createEmitterFromContainer( m_emitterName.c_str(), m_container );

		if( m_interface == 0 )
		{
			MENGE_LOG( "Error: Emitter can't create emitter source '%s'\n", m_resourcename.c_str() );
			return false;
		}

		m_interface->setListener( this );

		int count = m_interface->getNumTypes();

		for ( int i = count - 1; i >= 0; i-- )
		{
			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			std::string textureName = Holder<ParticleEngine>::hostage()->getTextureName();

			RenderImageInterface * image = m_resource->getRenderImage( textureName );

			if( image == 0 )
			{
				MENGE_LOG( "Error: Image can't loaded '%s'", textureName.c_str() );
				return false;
			}

			m_images.push_back( image );

			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_release()
	{
		Node::_release();

		Holder<ParticleEngine>::hostage()->releaseEmitter( m_interface );

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		m_images.clear();
		m_interface = NULL;
		m_resource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_render( const Viewport & _viewport )
	{
		int count = m_interface->getNumTypes();

		//const mt::mat3f & wm = getWorldMatrix();
		mt::mat3f wm;
		mt::ident_m3( wm );

		for ( int i = count - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			RenderImageInterface * image = m_images[i];

			if(m_interface->isIntensive())
			{
				m_blendDest = BF_ONE;
			}
			else
			{
				m_blendDest = BF_ONE_MINUS_SOURCE_ALPHA;
			}

			while ( nextParticleType == false )
			{
				RenderParticle * p = Holder<ParticleEngine>::hostage()->nextParticle();

				if( p == NULL )
				{
					nextParticleType = true;
				}
				else
				{
					// terrible :(
					Holder<RenderEngine>::hostage()->renderImage(
						wm, 
						mt::vec2f(p->x1, p->y1),
						mt::vec2f(p->x2, p->y2),
						mt::vec2f(p->x3, p->y3),
						mt::vec2f(p->x4, p->y4),
						mt::vec4f(p->u0, p->v0, p->u1, p->v1),
						p->color,
						image,
						m_blendSrc,
						m_blendDest
						);
					}
			}

			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::pause()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::stop()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->stop();
		this->callEvent( "EMITTER_STOP", "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setLooped( int _loop )
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->setLooped( _loop );
	}
	//////////////////////////////////////////////////////////////////////////
	int Emitter::getLooped() const
	{
		if( isActivate() == false )
		{
			return 0;
		}

		return m_interface->getLooped();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setLeftBorder( float _leftBorder )
	{
		if( isActivate() == false )
		{
			return;
		}

		return m_interface->setLeftBorder( _leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_update( float _timing )
	{
		const mt::vec2f & pos = getWorldPosition();

		m_interface->setPosition( pos.x, pos.y );
		m_interface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_debugRender( const Viewport & _viewport )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::onStopped()
	{
		this->callEvent( "EMITTER_END", "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_setListener()
	{
		Node::_setListener();

		registerEvent("EMITTER_END", "onEmitterEnd", m_listener );
		registerEvent("EMITTER_STOP", "onEmitterStop", m_listener );
	}
}