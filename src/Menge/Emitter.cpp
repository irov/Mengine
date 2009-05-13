#	include "Emitter.h" 

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "RenderEngine.h"
#	include "RenderObject.h"

#	include "Application.h"

#	include "ParticleEngine.h"

#	include "ResourceEmitterContainer.h"

#	include "ResourceManager.h"

#	include "math/box2.h"
#	include "Texture.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Emitter);
	//////////////////////////////////////////////////////////////////////////
	Emitter::Emitter()
		: m_interface( 0 )
		, m_resource( 0 )
		, m_autoPlay( false )
		, m_looped( false )
		, m_onEmitterEndEvent( false )
		, m_onEmitterStopEvent( false )
		, m_startPosition( 0.0f )
		, m_emitterRelative( false )
		, m_checkViewport( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Emitter::~Emitter()
	{
	}	
	///////////////////////////////////////////////////////////////////////////
	bool Emitter::_activate()
	{
		//bool enabled = Holder<Application>::hostage()->getParticlesEnabled();
		if( /*!enabled ||*/ Node::_activate() == false )
		{
			return false;
		}

		if( m_autoPlay )
		{
			m_interface->play();
			m_interface->update( m_startPosition );
			//play();
		}

		if( m_looped )
		{
			m_interface->setLooped( m_looped );
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
			XML_CASE_ATTRIBUTE_NODE( "AutoPlay", "Value", m_autoPlay );
			XML_CASE_ATTRIBUTE_NODE( "Looped", "Value", m_looped );
			XML_CASE_ATTRIBUTE_NODE( "StartPosition", "Value", m_startPosition );
			XML_CASE_ATTRIBUTE_NODE( "EmitterRelative", "Value", m_emitterRelative );
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
			MENGE_LOG_ERROR( "Emitter can't open resource file \"%s\""
				, m_resourcename.c_str() );
			return false;
		}

		const EmitterContainerInterface * m_container = m_resource->getContainer();

		if( m_container == NULL )
		{
			MENGE_LOG_ERROR( "Emitter can't open container file \"%s\""
				, m_resourcename.c_str() );
			return false;
		}

		m_interface = Holder<ParticleEngine>::hostage()
			->createEmitterFromContainer( m_emitterName, m_container );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( "Emitter can't create emitter source \"%s\""
				, m_resourcename.c_str() );
			return false;
		}

		m_interface->setListener( this );
		m_interface->setPosition( 0.0f, 0.0f );	// reset editor position

		int count = m_interface->getNumTypes();

		for ( int i = 0; i < count; ++i )
		{
			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			RenderObject* renderObject = Holder<RenderEngine>::hostage()->createRenderObject();

			String textureName = Holder<ParticleEngine>::hostage()->getTextureName();

			Texture* image = m_resource->getRenderImage( textureName );

			if( image == 0 )
			{
				MENGE_LOG_ERROR( "Image can't loaded \"%s\""
					, textureName.c_str() );
				return false;
			}

			//m_images.push_back( image );
			m_images.push_back( image );

			m_blendSrc = BF_SOURCE_ALPHA;
			if( m_interface->isIntensive() == true )
			{
				m_blendDst = BF_ONE;
			}
			else
			{
				m_blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
			}

			renderObject->material.primitiveType = PT_TRIANGLELIST;
			renderObject->material.textureStages = 1;
			renderObject->material.blendSrc = m_blendSrc;
			renderObject->material.blendDst = m_blendDst;
			renderObject->material.isSolidColor = false;
			renderObject->material.textureStage[0].texture = image;

			m_renderObjects.push_back( renderObject );

			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_release()
	{
		for( std::vector<RenderObject*>::iterator it = m_renderObjects.begin(), it_end = m_renderObjects.end();
			it != it_end;
			++it )
		{
			Holder<RenderEngine>::hostage()
				->releaseRenderObject( (*it) );
		}
		m_renderObjects.clear();
		m_images.clear();
		
		Node::_release();

		Holder<ParticleEngine>::hostage()->releaseEmitter( m_interface );

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		m_images.clear();
		m_interface = NULL;
		m_resource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_render( unsigned int _debugMask )
	{
		bool enabled = Holder<Application>::hostage()->getParticlesEnabled();
		if( !enabled )
		{
			return;
		}
		ParticleEngine* particleEngine = Holder<ParticleEngine>::hostage();

		Node::_render( _debugMask );

		ColourValue color = getWorldColor();

		int count = m_interface->getNumTypes();

		//RenderPass rPass;
		//rPass.blendDst = m_blendDst;
		//rPass.blendSrc = m_blendSrc;
		//rPass.textureStages = 1;
		//rPass.primitiveType = PT_TRIANGLELIST;
		mt::box2f pbox;
		const mt::mat3f& wm = getWorldMatrix();
		for ( int i = count - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			std::size_t count = 0;
			if( m_emitterRelative == true )
			{
				count = particleEngine->getParticlesCount( m_interface, i, m_checkViewport, &wm );
			}
			else
			{
				count = particleEngine->getParticlesCount( m_interface, i, m_checkViewport, NULL );
			}
			count = std::min<std::size_t>( count, 2000 );

			particleEngine->lockEmitter( m_interface, i );

			//RenderImageInterface * image = m_images[count - 1 - i];
			RenderObject* renderObject = m_renderObjects[i];
			//renderObject->vertices.clear();
			renderObject->vertices.resize( count * 4 );
			renderObject->material.indicies.clear();

			//rPass.textureStage[0].image = m_images[i];

			RenderParticle * p = particleEngine->nextParticle();
			std::size_t verticesNum = 0;
			while ( p != NULL && verticesNum < 2000 )
			{
				mt::vec2f vertices[4] =
				{
					mt::vec2f(p->x2, p->y2),
					mt::vec2f(p->x1, p->y1),
					mt::vec2f(p->x4, p->y4),
					mt::vec2f(p->x3, p->y3)
				};

				if( m_emitterRelative == true )
				{
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, vertices[0], wm );
					mt::mul_v2_m3_r( transformX, vertices[1] - vertices[0], wm );
					mt::mul_v2_m3_r( transformY, vertices[3] - vertices[0], wm );
					vertices[0] = origin;
					vertices[1] = vertices[0] + transformX;
					vertices[2] = vertices[1] + transformY;
					vertices[3] = vertices[0] + transformY;
				}

				mt::reset( pbox, vertices[0] );
				mt::add_internal_point( pbox, vertices[1] );
				mt::add_internal_point( pbox, vertices[2] );
				mt::add_internal_point( pbox, vertices[3] );

				if( m_checkViewport != NULL 
					&& 	m_checkViewport->testBBox( pbox ) == false )
				{
					p = particleEngine->nextParticle();
					continue;
				}

				const ColourValue& color = getWorldColor();
				ColourValue pColor;
				pColor.setAsARGB( p->color );
				ColourValue resColor = color * pColor;
				uint32 argb = resColor.getAsARGB();

				renderObject->material.indicies.push_back( 0 + verticesNum );
				renderObject->material.indicies.push_back( 3 + verticesNum );
				renderObject->material.indicies.push_back( 1 + verticesNum );
				renderObject->material.indicies.push_back( 1 + verticesNum );
				renderObject->material.indicies.push_back( 3 + verticesNum );
				renderObject->material.indicies.push_back( 2 + verticesNum );

				for( int i = 0; i < 4; i++ )
				{
					//renderObject->vertices.push_back( TVertex() );
					renderObject->vertices[verticesNum].pos[0] = vertices[i].x;
					renderObject->vertices[verticesNum].pos[1] = vertices[i].y;
					renderObject->vertices[verticesNum].color = argb;
					++verticesNum;
				}

				renderObject->vertices[verticesNum-4].uv[0] = p->u0;
				renderObject->vertices[verticesNum-4].uv[1] = p->v0;
				renderObject->vertices[verticesNum-3].uv[0] = p->u1;
				renderObject->vertices[verticesNum-3].uv[1] = p->v0;
				renderObject->vertices[verticesNum-2].uv[0] = p->u1;
				renderObject->vertices[verticesNum-2].uv[1] = p->v1;
				renderObject->vertices[verticesNum-1].uv[0] = p->u0;
				renderObject->vertices[verticesNum-1].uv[1] = p->v1;

				//renderObject->passes.push_back( rPass );

				p = particleEngine->nextParticle();
			}
			particleEngine->unlockEmitter( m_interface );

			Holder<RenderEngine>::hostage()->
				renderObject( renderObject );
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
		_update( m_startPosition );
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

		if( m_onEmitterStopEvent == true )
		{
			this->callEvent( EVENT_EMITTER_STOP, "(O)", this->getEmbedding() );
		}
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
		Node::_update( _timing );
		if( m_emitterRelative == false )
		{
			const mt::vec2f& pos = getWorldPosition();
			m_interface->setPosition( pos.x, pos.y );
			const mt::vec2f& dir = getWorldDirection();
			float rads = ::acosf( dir.x );
			if( dir.y > 0.0f ) rads = -rads;
			m_interface->setAngle( rads );
		}
		m_interface->update( _timing );
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::onStopped()
	{
		if( m_onEmitterEndEvent == true )
		{
			this->callEvent( EVENT_EMITTER_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_setListener()
	{
		Node::_setListener();

		m_onEmitterEndEvent = registerEvent( EVENT_EMITTER_END, ("onEmitterEnd"), m_listener );
		m_onEmitterStopEvent = registerEvent( EVENT_EMITTER_STOP, ("onEmitterStop"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::restart()
	{
		if( isActivate() == true )
		{
			m_interface->restart();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setResource( const String& _resourceName )
	{
		if( m_resourcename != _resourceName )
		{
			m_resourcename = _resourceName;
			if( isCompile() )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setEmitter( const String& _emitterName )
	{
		if( m_emitterName != _emitterName )
		{
			m_emitterName = _emitterName;
			if( isCompile() )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		if( isActivate() == false )
		{
			Node::_updateBoundingBox( _boundingBox );
			return;
		}

		bool reset = false;
		int count = m_interface->getNumTypes();

		for ( int i = count - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			RenderParticle * p = Holder<ParticleEngine>::hostage()->nextParticle();
			while ( p != NULL )
			{
				if( reset == false )
				{
					mt::reset( _boundingBox, mt::vec2f(p->x2, p->y2) );
					reset = true;
				}

				mt::vec2f vertices[4] =
				{
					mt::vec2f(p->x2, p->y2),
					mt::vec2f(p->x1, p->y1),
					mt::vec2f(p->x4, p->y4),
					mt::vec2f(p->x3, p->y3)
				};

				if( m_emitterRelative == true )
				{
					const mt::mat3f& wm = getWorldMatrix();
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, vertices[0], wm );
					mt::mul_v2_m3_r( transformX, vertices[1] - vertices[0], wm );
					mt::mul_v2_m3_r( transformY, vertices[3] - vertices[0], wm );
					vertices[0] = origin;
					vertices[1] = vertices[0] + transformX;
					vertices[2] = vertices[1] + transformY;
					vertices[3] = vertices[0] + transformY;
				}

				mt::add_internal_point( _boundingBox, vertices[0] );
				mt::add_internal_point( _boundingBox, vertices[1] );
				mt::add_internal_point( _boundingBox, vertices[2] );
				mt::add_internal_point( _boundingBox, vertices[3] );

				p = Holder<ParticleEngine>::hostage()->nextParticle();
			}
			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::playFromPosition( float _pos )
	{
		if( isActivate() == true )
		{
			m_interface->restart();
			m_interface->play();
			m_interface->update( _pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setEmitterRelative( bool _relative )
	{
		m_interface->setPosition( 0.0f, 0.0f );
		m_emitterRelative = _relative;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::_checkVisibility( const Viewport & _viewport )
	{
		static Viewport checkViewport = _viewport;
		m_checkViewport = &checkViewport;
		return Node::_checkVisibility( _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
}
