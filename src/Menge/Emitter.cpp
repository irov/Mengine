#	include "Emitter.h" 

#	include "ObjectImplement.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

#	include "Material.h"
#	include "Camera2D.h"

#	include "Player.h"

#	include "Application.h"

#	include "ParticleEngine.h"

#	include "ResourceEmitterContainer.h"

#	include "ResourceManager.h"

#	include "math/box2.h"
#	include "Texture.h"

namespace	Menge
{
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
			MENGE_LOG_ERROR( "Emitter can't open resource file '%s'"
				, m_resourcename.c_str() 
				);

			return false;
		}

		const EmitterContainerInterface * m_container = m_resource->getContainer();

		if( m_container == NULL )
		{
			MENGE_LOG_ERROR( "Emitter can't open container file '%s'"
				, m_resourcename.c_str() 
				);

			return false;
		}

		m_interface = Holder<ParticleEngine>::hostage()
			->createEmitterFromContainer( m_emitterName, m_container );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( "Emitter can't create emitter source '%s' - '%s'"
				, m_resourcename.c_str()
				, m_emitterName.c_str() 
				);

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

		for ( int i = 0; i < count; ++i )
		{
			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			Material* material = Holder<RenderEngine>::hostage()->createMaterial();

			m_imageOffsets.push_back( m_images.size() );

			int textureCount = Holder<ParticleEngine>::hostage()->getTextureCount();
			for( int i = 0; i < textureCount; ++i )
			{
				String textureName = Holder<ParticleEngine>::hostage()->getTextureName( i );

			
				ResourceImageDefault* image = m_resource->getRenderImage( textureName );
	
				if( image == 0 )
				{
					MENGE_LOG_ERROR( "Image can't loaded '%s' (ResourceName:%s, EmitterName:%s)"
						, textureName.c_str()
						, m_resourcename.c_str()
						, m_emitterName.c_str() 
						);

					return false;
				}
				m_images.push_back( image );
			}

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

		m_vertices.resize( count );

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
	void Emitter::_render( Camera2D * _camera )
	{
		bool enabled = Holder<Application>::hostage()->getParticlesEnabled();
		if( !enabled )
		{
			return;
		}	

		ParticleEngine* particleEngine = Holder<ParticleEngine>::hostage();

		Node::_render( _camera );

		size_t partCount = 0;
		std::size_t maxParticleCount = particleEngine->getMaxParticlesCount();

		int typeCount = m_interface->getNumTypes();
		
		const Viewport & vp = _camera->getViewport();

		for( int i = typeCount - 1; i >= 0; i-- )
		{
			for( TVectorBatchs::iterator
				it = m_batchs.begin(),
				it_end = m_batchs.end();
			it != it_end;
			++it )
			{
				Batch & batch = *it;
				Holder<RenderEngine>::hostage()->
					renderObject2D( m_materials[i], &batch.texture, 1, &m_vertices[batch.it_begin], batch.it_end - batch.it_begin, LPT_QUAD );
			}
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
	void Emitter::setAutoPlay( bool _autoPlay )
	{
		m_autoPlay = _autoPlay;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::getAutoPlay() const
	{
		return m_autoPlay;
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

		const mt::mat3f& wm = getWorldMatrix();

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

		ParticleEngine* particleEngine = Holder<ParticleEngine>::hostage();

		std::size_t partCount = 0;
		std::size_t maxParticleCount = particleEngine->getMaxParticlesCount();

		int typeCount = m_interface->getNumTypes();

		bool firstPoint = true;

		Node::_updateBoundingBox( m_boundingBox );

		const Viewport & vp = Player::hostage()->getRenderCamera2D()->getViewport();

		const ColourValue & color = getWorldColor();
		ARGB color_argb = color.getAsARGB();

		static TVectorRenderParticle s_particles(maxParticleCount);
		m_vertices.clear();
		m_batchs.clear();

		for( int i = 0; i != typeCount; ++i )
		{
			bool nextParticleType = false;

			int texturesNum = 0;
			int particlesNum = 0;
			if( particleEngine->flushEmitter( m_interface, i, s_particles, texturesNum, particlesNum, maxParticleCount ) == false )
			{
				continue;
			}

			m_vertices.resize( m_vertices.size() + particlesNum * 4 );

			for( RenderParticle *
				it = &s_particles[0],
				*it_end = &s_particles[particlesNum];
			it != it_end && partCount < maxParticleCount;
			++it )
			{
				RenderParticle & p = *it;

				EmitterRectangle& eq = reinterpret_cast<EmitterRectangle&>(p.rectangle);

				if( m_emitterRelative == true )
				{
					mt::vec2f origin, transformX, transformY;
					mt::mul_v2_m3( origin, eq.v[0], wm );
					mt::mul_v2_m3_r( transformX, eq.v[1] - eq.v[0], wm );
					mt::mul_v2_m3_r( transformY, eq.v[3] - eq.v[0], wm );
					eq.v[0] = origin;
					eq.v[1] = eq.v[0] + transformX;
					eq.v[2] = eq.v[1] + transformY;
					eq.v[3] = eq.v[0] + transformY;
				}

				int ioffset = m_imageOffsets[i];
				ResourceImageDefault* image = m_images[ioffset+p.texture.frame];
				
				const mt::vec2f& offset = image->getOffset( 0 );
				const mt::vec2f& size = image->getSize( 0 );
				const mt::vec2f& maxSize = image->getMaxSize( 0 );
				float dx1 = offset.x / maxSize.x;
				float dy1 = offset.y / maxSize.y;
				float dx2 = 1.0f - (offset.x + size.x) / maxSize.x;
				float dy2 = 1.0f - (offset.y + size.y) / maxSize.y;

				mt::vec2f axisX( eq.v[1] - eq.v[0] );
				mt::vec2f axisY( eq.v[3] - eq.v[0] );

				eq.v[0] += axisX * dx1 + axisY * dy1;
				eq.v[1] += -axisX * dx2 + axisY * dy1;
				eq.v[2] += -axisX * dx2 - axisY * dy2;
				eq.v[3] += axisX * dx1 - axisY * dy2;

				mt::box2f pbox;
				mt::reset( pbox, eq.v[0] );
				mt::add_internal_point( pbox, eq.v[1] );
				mt::add_internal_point( pbox, eq.v[2] );
				mt::add_internal_point( pbox, eq.v[3] );

				if( vp.testBBox( pbox ) == false )
				{
					continue;
				}

				if( firstPoint == true )
				{
					firstPoint = false;
					m_boundingBox = pbox; 
				}
				else
				{
					mt::merge_box( m_boundingBox, pbox );
				}

				uint32 argb;

				if( color.isIdentity() )
				{
					argb = p.color.rgba;
				}
				else if( p.color.rgba == 0xFFFFFFFF )
				{
					argb = color_argb;
				}
				else
				{
					ColourValue cv( ARGB(p.color.rgba) );
					cv *= color;
					argb = cv.getAsARGB();
				}

				Vertex2D * vertice = &m_vertices[partCount * 4];
				
				for( int j = 0; j != 4; ++j )
				{
					//renderObject->vertices.push_back( TVertex() );
					vertice[j].pos[0] = eq.v[j].x;
					vertice[j].pos[1] = eq.v[j].y;
					vertice[j].color = argb;
				}

				const mt::vec4f& uv = image->getUV( 0 );

				vertice[0].uv[0] = uv.x;
				vertice[0].uv[1] = uv.y;
				vertice[1].uv[0] = uv.z;
				vertice[1].uv[1] = uv.y;
				vertice[2].uv[0] = uv.z;
				vertice[2].uv[1] = uv.w;
				vertice[3].uv[0] = uv.x;
				vertice[3].uv[1] = uv.w;

				//m_vertices.insert( m_vertices.begin(), vertice, vertice + 4 );

				Texture* texture = image->getTexture( 0 );

				if( m_batchs.empty() )
				{
					Batch batch;
					batch.it_begin = 0;
					batch.it_end = partCount * 4;
					batch.texture = texture;
					m_batchs.push_back( batch );
				}
				else
				{
					Batch & prev = m_batchs.back();
					if( prev.texture == texture )
					{
						prev.it_end = partCount * 4;
					}
					else
					{
						Batch batch;
						batch.it_begin = prev.it_end;
						batch.it_end = partCount * 4;
						batch.texture = texture;
						m_batchs.push_back( batch );
					}
				}
				
				++partCount;
			}
		}

		this->invalidateBoundingBox();
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
		if( m_resourcename == _resourceName )
		{
			return;
		}
		
		m_resourcename = _resourceName;
		
		if( isCompile() == false )
		{
			return;
		}
	
		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::setEmitter( const String& _emitterName )
	{
		if( m_emitterName == _emitterName )
		{
			return;
		}
		
		m_emitterName = _emitterName;

		if( isCompile() == false )
		{
			return;
		}
		
		recompile();
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
	void Emitter::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		//Empty
	}
}
