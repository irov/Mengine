#	include "ParticleEmitter.h" 

#	include "BinParser.h"

#	include "Logger/Logger.h"
#	include "RenderEngine.h"
#	include "EventManager.h"

#	include "Material.h"
#	include "Camera2D.h"

#	include "Player.h"

#	include "Scene.h"
#	include "Layer.h"

#	include "Application.h"

#	include "ParticleEngine.h"

#	include "ResourceEmitterContainer.h"

#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"

#	include "Sprite.h"
#	include "Consts.h"

#	include "math/box2.h"
#	include "math/angle.h"
#	include "math/triangulation.h"

#	include "Texture.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter::ParticleEmitter()
		: m_interface(0)
		, m_resource(0)
		, m_startPosition(0.f)
		, m_emitterRelative(false)
		, m_centerAlign(false)
		, m_checkViewport(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter::~ParticleEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		Eventable::registerEvent( EVENT_PARTICLE_EMITTER_END, ("onParticleEmitterEnd"), _listener );
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_activate()
	{
		//bool enabled = Application::get()->getParticlesEnabled();
		if( /*!enabled ||*/ Node::_activate() == false )
		{
			return false;
		}

		if( m_emitterRelative == true )
		{
			mt::vec2f pos;

			m_interface->getPosition( pos );

			Scene * scene = this->getScene();
			Layer * mainLayer = scene->getMainLayer();

			const mt::vec2f & layerSize = mainLayer->getSize();

			pos.x += layerSize.x * 0.5f;
			pos.y += layerSize.y * 0.5f;

			m_interface->setPosition( pos );
		}
		else
		{
			const mt::vec2f& pos = this->getWorldPosition();
			m_interface->setPosition( pos );
			
			const mt::vec2f& dir = this->getWorldDirection();
			float angle = mt::signed_angle( dir );
			m_interface->setAngle( angle );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_deactivate()
	{
		Node::_deactivate();

		this->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::loader( BinParser * _parser )
	{
		Node::loader( _parser );

		bool depricated_autoPlay;
		bool depricated_loop;

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Resource_Name, m_resourcename );
			BIN_CASE_ATTRIBUTE( Protocol::Emitter_Name, m_emitterName );
			BIN_CASE_ATTRIBUTE( Protocol::AutoPlay_Value, depricated_autoPlay ); //depricated
			BIN_CASE_ATTRIBUTE( Protocol::CenterAlign_Value, m_centerAlign );
			BIN_CASE_ATTRIBUTE( Protocol::Looped_Value, depricated_loop ); //depricated
			BIN_CASE_ATTRIBUTE( Protocol::StartPosition_Value, m_startPosition );
			BIN_CASE_ATTRIBUTE( Protocol::EmitterRelative_Value, m_emitterRelative );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = ResourceManager::get()
			->getResourceT<ResourceEmitterContainer>( m_resourcename );

		if( m_resource == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEmitter '%s' can't open resource file '%s'"
				, m_name.c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}

		EmitterContainerInterface * container = m_resource->getContainer();

		if( container == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEmitter can't open container file '%s'"
				, m_resourcename.c_str() 
				);

			return false;
		}

		m_interface = container->createEmitter( m_emitterName.to_str() );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( "ParticleEmitter '%s' can't create emitter source '%s' - '%s'"
				, m_name.c_str()
				, m_resourcename.c_str()
				, m_emitterName.c_str() 
				);

			return false;
		}

		m_interface->setListener( this );

		//// reset editor position
		//if( m_emitterRelative == false )
		//{
		//	const mt::vec2f& pos = getWorldPosition();
		//	m_interface->setPosition( pos.x, pos.y );
		//	const mt::vec2f& dir = getWorldDirection();
		//	float rads = ::acosf( dir.x );
		//	if( dir.y > 0.0f ) rads = -rads;
		//	m_interface->setAngle( rads );
		//}

		const MaterialGroup * mg_intensive = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(ParticleIntensive) );

		const MaterialGroup * mg_nonintensive = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(ParticleBlend) );

		m_materials[0] = mg_intensive->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_materials[1] = mg_nonintensive->getMaterial( TAM_CLAMP, TAM_CLAMP );

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_release()
	{
		Node::_release();
		
		if( m_interface )
		{
			EmitterContainerInterface * container = 
				m_resource->getContainer();

			container->releaseEmitter( m_interface );
		}

		ResourceManager::get()->releaseResource( m_resource );

		//m_images.clear();
		m_interface = NULL;
		m_resource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		bool enabled = Application::get()
			->getParticlesEnabled();

		if( enabled == false || this->isPlay() == false)
		{
			return;
		}	

		size_t partCount = 0;
		std::size_t maxParticleCount = ParticleEngine::get()
			->getMaxParticlesCount();
		
		for( TVectorBatchs::iterator
			it = m_batchs.begin(),
			it_end = m_batchs.end();
		it != it_end;
		++it )
		{
			Batch & batch = *it;

			RenderEngine::get()->
				renderObject2D( batch.material, batch.texture, NULL, 1, &m_vertices[batch.begin], batch.size, LPT_QUAD );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_play()
	{
		if( isActivate() == false )
		{
			return false;
		}

		m_interface->play();

		ParticleEmitter::_update( m_startPosition );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::pause()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_stop( std::size_t _enumerator )
	{
		if( isActivate() == false )
		{
			return;
		}

		this->callEventDeferred(EVENT_PARTICLE_EMITTER_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(false) );

		m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_end( std::size_t _enumerator )
	{
		this->callEventDeferred( EVENT_PARTICLE_EMITTER_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setLoop( bool _value )
	{
		if( isActivate() == false )
		{
			return;
		}

		m_interface->setLoop(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setLeftBorder( float _leftBorder )
	{
		if( isActivate() == false )
		{
			return;
		}

		return m_interface->setLeftBorder( _leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_update( float _timing )
	{
		Node::_update( _timing );

		bool enabled = Application::get()
			->getParticlesEnabled();

		if( enabled == false || this->isPlay() == false)
		{
			return;
		}

		//const mt::mat3f& wm = getWorldMatrix();

		m_interface->update( _timing );

		ParticleEngine* particleEngine = ParticleEngine::get();

		std::size_t partCount = 0;
		std::size_t maxParticleCount = particleEngine->renderParticlesCount(0);

		bool firstPoint = true;

		Node::_updateBoundingBox( m_boundingBox );

		const Viewport & vp = Player::get()->getRenderCamera2D()->getViewport();

		const ColourValue & color = getWorldColor();
		ARGB color_argb = color.getAsARGB();

		static TVectorParticleVerices s_particles(maxParticleCount);
		static TVectorParticleMeshes s_meshes(20);
		s_particles.clear();
		s_meshes.clear();

		m_vertices.clear();
		m_batchs.clear();

		if( particleEngine->flushEmitter( m_interface, s_meshes, s_particles, maxParticleCount ) == false )
		{
			return;
		}

		m_vertices.resize( s_particles.size() * 4 );

		for( TVectorParticleMeshes::const_iterator
			it = s_meshes.begin(),
			it_end = s_meshes.end();
		it != it_end;
		++it )
		{
				//if( m_emitterRelative == false )
				//{
				//	const mt::mat3f & wm = this->getWorldMatrix();

				//	mt::vec2f origin, transformX, transformY;
				//	mt::mul_v2_m3( origin, p.v[0], wm );
				//	mt::mul_v2_m3_r( transformX, p.v[1] - p.v[0], wm );
				//	mt::mul_v2_m3_r( transformY, p.v[3] - p.v[0], wm );
				//	p.v[0] = origin;
				//	p.v[1] = p.v[0] + transformX;
				//	p.v[2] = p.v[1] + transformY;
				//	p.v[3] = p.v[0] + transformY;
				//}
			const ParticleMesh & mesh = *it;

			ResourceImageDefault * image = m_resource->getAtlasImage( mesh.texture );

			const ResourceImage::ImageFrame & frame = image->getImageFrame( 0 );

			const mt::vec2f& offset = frame.offset;
			const mt::vec2f& size = frame.size;
			const mt::vec2f& maxSize = frame.maxSize;
			float dx1 = offset.x / maxSize.x;
			float dy1 = offset.y / maxSize.y;
			float dx2 = 1.0f - (offset.x + size.x) / maxSize.x;
			float dy2 = 1.0f - (offset.y + size.y) / maxSize.y;

			for( TVectorParticleVerices::size_type
				it = mesh.begin,
				it_end = mesh.begin + mesh.size;
			it != it_end;
			++it )
			{
				ParticleVertices & p = s_particles[it];

				mt::vec2f axisX( p.v[1] - p.v[0] );
				mt::vec2f axisY( p.v[3] - p.v[0] );

				p.v[0] += axisX * dx1 + axisY * dy1;
				p.v[1] += -axisX * dx2 + axisY * dy1;
				p.v[2] += -axisX * dx2 - axisY * dy2;
				p.v[3] += axisX * dx1 - axisY * dy2;

				mt::box2f pbox;
				mt::reset( pbox, p.v[0] );
				mt::add_internal_point( pbox, p.v[1] );
				mt::add_internal_point( pbox, p.v[2] );
				mt::add_internal_point( pbox, p.v[3] );

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
					argb = p.color;
				}
				else if( p.color == 0xFFFFFFFF )
				{
					argb = color_argb;
				}
				else
				{
					ColourValue cv( ARGB(p.color) );
					cv *= color;
					argb = cv.getAsARGB();
				}

				Vertex2D * vertice = &m_vertices[it * 4];
				
				for( int j = 0; j != 4; ++j )
				{
					//renderObject->vertices.push_back( TVertex() );
					vertice[j].pos[0] = p.v[j].x;
					vertice[j].pos[1] = p.v[j].y;
					vertice[j].color = argb;
				}

				const mt::vec4f& uv = frame.uv;

				vertice[0].uv[0] = p.uv[0].x;
				vertice[0].uv[1] = p.uv[0].y;
				vertice[1].uv[0] = p.uv[1].x;
				vertice[1].uv[1] = p.uv[1].y;
				vertice[2].uv[0] = p.uv[2].x;
				vertice[2].uv[1] = p.uv[2].y;
				vertice[3].uv[0] = p.uv[3].x;
				vertice[3].uv[1] = p.uv[3].y;
			}

			Texture* texture = frame.texture;

			//++partCount;

			Batch batch;
			batch.begin = mesh.begin * 4;
			batch.size = mesh.size * 4;
			batch.texture[0] = texture;
			batch.material = mesh.intense ? m_materials[0] : m_materials[1];

			m_batchs.push_back( batch );
		}

		particleEngine->renderParticlesCount(partCount);

		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::onParticleEmitterStopped()
	{
		this->end();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::restart()
	{
		if( isActivate() == true )
		{
			m_interface->restart();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setResource( const ConstString& _resourceName )
	{
		if( m_resourcename == _resourceName )
		{
			return;
		}
		
		m_resourcename = _resourceName;
		
		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitter( const ConstString& _emitterName )
	{
		if( m_emitterName == _emitterName )
		{
			return;
		}
		
		m_emitterName = _emitterName;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::playFromPosition( float _pos )
	{
		if( isActivate() == false )
		{
			return;
		}

		//m_interface->setLeftBorder( _pos );
		m_interface->restart();
		m_interface->play();
		m_interface->update( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterRelative( bool _relative )
	{
		m_emitterRelative = _relative;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setStartPosition( float _pos )
	{
		m_startPosition = _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::changeEmitterImage( ResourceImage * _resourceImage )
	{
		if( this->compile() == false )
		{
			MENGE_LOG_ERROR("ParticleEmitter::changeEmitterImage invalid not compile '%s'"
				, m_name.c_str()
				);

			return;
		}

		size_t alphaWidth = 0;
		size_t alphaHeight = 0;

		unsigned char * alphaMap = AlphaChannelManager::get()
			->getAlphaBuffer( _resourceImage, 0, alphaWidth, alphaHeight );

		m_interface->changeEmitterImage( alphaWidth, alphaHeight, alphaMap, 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::changeEmitterPolygon( const mt::polygon & _polygon )
	{
		if( this->compile() == false )
		{
			MENGE_LOG_ERROR("ParticleEmitter::changeEmitterPolygon invalid not compile '%s'"
				, m_name.c_str()
				);

			return;
		}

		mt::TVectorPoints points;
		mt::triangulate_polygon( _polygon, points );

		if( points.empty() == true )
		{
			return;
		}

		m_interface->changeEmitterModel( points.front().buff(), points.size() / 3 );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_invalidateWorldMatrix()
	{
		if( m_interface == 0 )
		{
			return;
		}

		if( m_emitterRelative == false )
		{
			const mt::vec2f& pos = this->getWorldPosition();
			m_interface->setPosition( pos );
			
			const mt::vec2f& dir = this->getWorldDirection();
			float angle = mt::signed_angle( dir );
			m_interface->setAngle( angle );
		}
	}
}
