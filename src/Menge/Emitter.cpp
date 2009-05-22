#	include "Emitter.h" 

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "RenderEngine.h"
#	include "Material.h"

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

			Material* material = Holder<RenderEngine>::hostage()->createMaterial();

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

			material->textureStages = 1;
			material->blendSrc = m_blendSrc;
			material->blendDst = m_blendDst;
			material->isSolidColor = false;
			material->textureStage[0].texture = image;

			m_materials.push_back( material );

			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}
		m_vertices.resize( m_materials.size() );

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_release()
	{
		for( TMaterialVector::iterator it = m_materials.begin(), it_end = m_materials.end();
			it != it_end;
			++it )
		{
			Holder<RenderEngine>::hostage()
				->releaseMaterial( (*it) );
		}
		m_materials.clear();
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

			TVertex2DVector& vertices = m_vertices[i];
			vertices.resize( count * 4 );

			RenderParticle p;
			std::size_t verticesNum = 0;
			while ( particleEngine->nextParticle( p ) && verticesNum < 2000 )
			{
				mt::vec2f tvertices[4] =
				{
					mt::vec2f(p.x2, p.y2),
					mt::vec2f(p.x1, p.y1),
					mt::vec2f(p.x4, p.y4),
					mt::vec2f(p.x3, p.y3)
				};

				if( m_emitterRelative == true )
				{
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, tvertices[0], wm );
					mt::mul_v2_m3_r( transformX, tvertices[1] - tvertices[0], wm );
					mt::mul_v2_m3_r( transformY, tvertices[3] - tvertices[0], wm );
					tvertices[0] = origin;
					tvertices[1] = tvertices[0] + transformX;
					tvertices[2] = tvertices[1] + transformY;
					tvertices[3] = tvertices[0] + transformY;
				}

				mt::reset( pbox, tvertices[0] );
				mt::add_internal_point( pbox, tvertices[1] );
				mt::add_internal_point( pbox, tvertices[2] );
				mt::add_internal_point( pbox, tvertices[3] );

				if( m_checkViewport != NULL 
					&& 	m_checkViewport->testBBox( pbox ) == false )
				{
					continue;
				}

				const ColourValue& color = getWorldColor();
				ColourValue pColor;
				pColor.setAsARGB( p.color );
				ColourValue resColor = color * pColor;
				uint32 argb = resColor.getAsARGB();

				for( int i = 0; i < 4; i++ )
				{
					//renderObject->vertices.push_back( TVertex() );
					vertices[verticesNum].pos[0] = tvertices[i].x;
					vertices[verticesNum].pos[1] = tvertices[i].y;
					vertices[verticesNum].color = argb;
					++verticesNum;
				}

				vertices[verticesNum-4].uv[0] = p.u0;
				vertices[verticesNum-4].uv[1] = p.v0;
				vertices[verticesNum-3].uv[0] = p.u1;
				vertices[verticesNum-3].uv[1] = p.v0;
				vertices[verticesNum-2].uv[0] = p.u1;
				vertices[verticesNum-2].uv[1] = p.v1;
				vertices[verticesNum-1].uv[0] = p.u0;
				vertices[verticesNum-1].uv[1] = p.v1;

				//renderObject->passes.push_back( rPass );
			}
			particleEngine->unlockEmitter( m_interface );

			Holder<RenderEngine>::hostage()->
				renderObject2D( m_materials[i], &(vertices[0]), vertices.size(), LPT_QUAD );
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

			RenderParticle p;
			while ( Holder<ParticleEngine>::hostage()->nextParticle( p ) )
			{
				mt::vec2f vertices[4] =
				{
					mt::vec2f(p.x2, p.y2),
					mt::vec2f(p.x1, p.y1),
					mt::vec2f(p.x4, p.y4),
					mt::vec2f(p.x3, p.y3)
				};

				if( reset == false )
				{
					mt::reset( _boundingBox, vertices[0] );
					reset = true;
				}

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
