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
	static TVectorRenderParticle s_cacheParticles;
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Emitter);
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

		if( m_looped )
		{
			m_interface->setLooped( m_looped );
		}

		if( m_autoPlay )
		{
			m_interface->play();
			m_interface->update( m_startPosition );
			//m_interface->setLeftBorder( m_startPosition/* 1000.0f */);
			//play();
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
			MENGE_LOG_ERROR( "Emitter can't create emitter source \"%s\" - \"%s\""
				, m_resourcename.c_str(), m_emitterName.c_str() );
			return false;
		}

		m_interface->setListener( this );

		// reset editor position
		if( m_emitterRelative == false )
		{
			const mt::vec2f& pos = getWorldPosition();
			m_interface->setPosition( pos.x, pos.y );
			const mt::vec2f& dir = getWorldDirection();
			float rads = ::acosf( dir.x );
			if( dir.y > 0.0f ) rads = -rads;
			m_interface->setAngle( rads );
		}
		else
		{
			m_interface->setPosition( 0.0f, 0.0f );
		}

		int count = m_interface->getNumTypes();
		m_imageOffsets.push_back( 0 );
		for ( int i = 0; i < count; ++i )
		{
			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			Material* material = Holder<RenderEngine>::hostage()->createMaterial();

			int textureCount = Holder<ParticleEngine>::hostage()->getTextureCount();
			for( int i = 0; i < textureCount; ++i )
			{
				String textureName = Holder<ParticleEngine>::hostage()->getTextureName( i );

			
				ResourceImageDefault* image = m_resource->getRenderImage( textureName );
	
				if( image == 0 )
				{
					MENGE_LOG_ERROR( "Image can't loaded \"%s\" (ResourceName:%s, EmitterName:%s)"
						, textureName.c_str(), m_resourcename.c_str(), m_emitterName.c_str() );
					return false;
				}
				m_images.push_back( image );
			}
			m_imageOffsets.push_back( m_images.size() );

			//m_images.push_back( image );
			//m_images.push_back( image );

			m_blendSrc = BF_SOURCE_ALPHA;
			if( m_interface->isIntensive() == true )
			{
				m_blendDst = BF_ONE;
			}
			else
			{
				m_blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
			}

			//material->textureStages = 1;
			material->blendSrc = m_blendSrc;
			material->blendDst = m_blendDst;
			material->isSolidColor = false;
			//material->textureStage[0].texture = image;
			material->textureStage[0].colorOp = TOP_MODULATE;
			material->textureStage[0].alphaOp = TOP_MODULATE;

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
		m_imageOffsets.clear();
		
		Node::_release();

		Holder<ParticleEngine>::hostage()->releaseEmitter( m_interface );

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		//m_images.clear();
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

		int typeCount = m_interface->getNumTypes();

		mt::box2f pbox;
		const mt::mat3f& wm = getWorldMatrix();
		for ( int i = typeCount - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			s_cacheParticles.clear();

			int texturesNum = 0;
			bool flushResult = particleEngine->flushEmitter( m_interface, i, s_cacheParticles, &texturesNum );
			if( flushResult == false || texturesNum == 0 )
			{
				continue;
			}

			std::size_t particleCount = 0;
			if( m_emitterRelative == true )
			{
				particleCount = particleEngine->getParticlesCount( s_cacheParticles, m_interface, i, m_checkViewport, &wm );
			}
			else
			{
				particleCount = particleEngine->getParticlesCount( s_cacheParticles, m_interface, i, m_checkViewport, NULL );
			}

			//particleEngine->lockEmitter( m_interface, i );

			TVertex2DVector& vertices = m_vertices[i];
			vertices.resize( particleCount * 4 );

			std::size_t verticesNum = 0;
			size_t partCount = 0;

			for( TVectorRenderParticle::iterator
				it = s_cacheParticles.begin(),
				it_end = s_cacheParticles.end();
			it != it_end && partCount != particleCount;
			++it )
			{
				RenderParticle & p = *it;

				EmitterRectangle& eq = reinterpret_cast<EmitterRectangle&>(p.rectangle);

				/*if( m_emitterRelative == true )
				{
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, eq.v[0], wm );
					mt::mul_v2_m3_r( transformX, eq.v[1] - eq.v[0], wm );
					mt::mul_v2_m3_r( transformY, eq.v[3] - eq.v[0], wm );
					eq.v[0] = origin;
					eq.v[1] = eq.v[0] + transformX;
					eq.v[2] = eq.v[1] + transformY;
					eq.v[3] = eq.v[0] + transformY;
				}*/

				mt::reset( pbox, eq.v[0] );
				mt::add_internal_point( pbox, eq.v[1] );
				mt::add_internal_point( pbox, eq.v[2] );
				mt::add_internal_point( pbox, eq.v[3] );

				if( m_checkViewport != NULL 
					&& 	m_checkViewport->testBBox( pbox ) == false )
				{
					continue;
				}

				const ColourValue& color = getWorldColor();
				ColourValue pColor;
				pColor.setAsARGB( p.color.rgba );
				ColourValue resColor = color * pColor;
				uint32 argb = resColor.getAsARGB();
				
				int ioffset = m_imageOffsets[i];
				ResourceImageDefault* image = m_images[ioffset+p.texture.frame];
				const mt::vec4f& uv = image->getUV( 0 );
				const mt::vec2f& offset = image->getOffset( 0 );
				const mt::vec2f& size = image->getSize( 0 );
				const mt::vec2f& maxSize = image->getMaxSize( 0 );
				float dx1 = offset.x / maxSize.x;
				float dy1 = offset.y / maxSize.y;
				float dx2 = 1.0f - (offset.x + size.x) / maxSize.x;
				float dy2 = 1.0f - (offset.y + size.y) / maxSize.y;
				Texture* texture = image->getImage( 0 );

				mt::vec2f axisX( eq.v[1] - eq.v[0] );
				mt::vec2f axisY( eq.v[3] - eq.v[0] );

				eq.v[0] += axisX * dx1 + axisY * dy1;
				eq.v[1] += -axisX * dx2 + axisY * dy1;
				eq.v[2] += -axisX * dx2 - axisY * dy2;
				eq.v[3] += axisX * dx1 - axisY * dy2;

				for( int j = 0; j < 4; j++ )
				{
					//renderObject->vertices.push_back( TVertex() );
					vertices[verticesNum].pos[0] = eq.v[j].x;
					vertices[verticesNum].pos[1] = eq.v[j].y;
					vertices[verticesNum].color = argb;
					++verticesNum;
				}


				vertices[verticesNum-4].uv[0] = uv.x;
				vertices[verticesNum-4].uv[1] = uv.y;
				vertices[verticesNum-3].uv[0] = uv.z;
				vertices[verticesNum-3].uv[1] = uv.y;
				vertices[verticesNum-2].uv[0] = uv.z;
				vertices[verticesNum-2].uv[1] = uv.w;
				vertices[verticesNum-1].uv[0] = uv.x;
				vertices[verticesNum-1].uv[1] = uv.w;

				//renderObject->passes.push_back( rPass );
				++partCount;

				Holder<RenderEngine>::hostage()->
					renderObject2D( m_materials[i], &texture, 1, &(vertices[verticesNum-4]), 4, LPT_QUAD );
			}

			//particleEngine->unlockEmitter( m_interface );

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
	void Emitter::setLooped( bool _loop )
	{
		m_looped = _loop;

		if( isActivate() == true )
		{
			m_interface->setLooped( _loop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::getLooped() const
	{
		return m_looped;
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

		m_onEmitterEndEvent = Eventable::registerEvent( EVENT_EMITTER_END, ("onEmitterEnd"), m_listener );
		m_onEmitterStopEvent = Eventable::registerEvent( EVENT_EMITTER_STOP, ("onEmitterStop"), m_listener );
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

		ParticleEngine* particleEngine = Holder<ParticleEngine>::hostage();

		bool reset = false;
		int count = m_interface->getNumTypes();

		for ( int i = count - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			s_cacheParticles.clear();
			if( particleEngine->flushEmitter( m_interface, i, s_cacheParticles, NULL ) == false )
			{
				continue;
			}
			
			for( TVectorRenderParticle::iterator
				it = s_cacheParticles.begin(),
				it_end = s_cacheParticles.end();
			it != it_end;
			++it )
			{
				RenderParticle & p = *it;

				EmitterRectangle& rectangle = reinterpret_cast<EmitterRectangle&>(p.rectangle);

				if( reset == false )
				{
					mt::reset( _boundingBox, rectangle.v[0] );
					reset = true;
				}

				if( m_emitterRelative == true )
				{
					const mt::mat3f& wm = getWorldMatrix();
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, rectangle.v[0], wm );
					mt::mul_v2_m3_r( transformX, rectangle.v[1] - rectangle.v[0], wm );
					mt::mul_v2_m3_r( transformY, rectangle.v[3] - rectangle.v[0], wm );
					rectangle.v[0] = origin;
					rectangle.v[1] = rectangle.v[0] + transformX;
					rectangle.v[2] = rectangle.v[1] + transformY;
					rectangle.v[3] = rectangle.v[0] + transformY;
				}

				mt::add_internal_point( _boundingBox, rectangle.v[0] );
				mt::add_internal_point( _boundingBox, rectangle.v[1] );
				mt::add_internal_point( _boundingBox, rectangle.v[2] );
				mt::add_internal_point( _boundingBox, rectangle.v[3] );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::playFromPosition( float _pos )
	{
		if( isActivate() == true )
		{
			//m_interface->setLeftBorder( _pos );
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
