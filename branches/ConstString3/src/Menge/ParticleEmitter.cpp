#	include "ParticleEmitter.h" 

#	include "BinParser.h"

#	include "LogEngine.h"
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

#	include "ResourceImageDefault.h"

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

		this->updateRelative_();

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

		if( m_emitterImageName.empty() == false )
		{
			if( this->compileEmitterImage_() == false )
			{
				return false;
			}
		}

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
		if( this->isActivate() == false )
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
		if( this->isActivate() == false )
		{
			return;
		}

		m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_stop( std::size_t _enumerator )
	{
		if( this->isActivate() == false )
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
		if( this->isCompile() == false )
		{
			return;
		}

		m_interface->setLoop(_value);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setLeftBorder( float _leftBorder )
	{
		if( isCompile() == false )
		{
			return;
		}

		return m_interface->setLeftBorder( _leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_update( float _timing )
	{
		Node::_update( _timing );

		if( this->isPlay() == false )
		{
			return;
		}

		bool enabled = Application::get()
			->getParticlesEnabled();

		if( enabled == false )
		{
			return;
		}

		m_interface->update( _timing );

		std::size_t partCount = 0;
		std::size_t maxParticleCount = ParticleEngine::get()
			->renderParticlesCount(0);

		bool firstPoint = true;

		Node::_updateBoundingBox( m_boundingBox );

		const ColourValue & color = this->getWorldColor();
		ARGB color_argb = color.getAsARGB();

		static TVectorParticleVerices s_particles(maxParticleCount);
		static TVectorParticleMeshes s_meshes(20);
		s_particles.clear();
		s_meshes.clear();

		m_vertices.clear();
		m_batchs.clear();

		if( ParticleEngine::get()
			->flushEmitter( m_interface, s_meshes, s_particles, maxParticleCount ) == false )
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
			const ParticleMesh & mesh = *it;

			for( TVectorParticleVerices::size_type
				it = mesh.begin,
				it_end = mesh.begin + mesh.size;
			it != it_end;
			++it )
			{
				const ParticleVertices & p = s_particles[it];

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

				vertice[0].pos[0] = p.v[0].x;
				vertice[0].pos[1] = p.v[0].y;
				vertice[0].pos[2] = 0.f;
				vertice[0].pos[3] = 1.f;

				vertice[0].color = argb;

				vertice[1].pos[0] = p.v[1].x;
				vertice[1].pos[1] = p.v[1].y;
				vertice[1].pos[2] = 0.f;
				vertice[1].pos[3] = 1.f;

				vertice[1].color = argb;

				vertice[2].pos[0] = p.v[2].x;
				vertice[2].pos[1] = p.v[2].y;
				vertice[2].pos[2] = 0.f;
				vertice[2].pos[3] = 1.f;

				vertice[2].color = argb;

				vertice[3].pos[0] = p.v[3].x;
				vertice[3].pos[1] = p.v[3].y;
				vertice[3].pos[2] = 0.f;
				vertice[3].pos[3] = 1.f;

				vertice[3].color = argb;

				vertice[0].uv[0] = p.uv[0].x;
				vertice[0].uv[1] = p.uv[0].y;
				vertice[1].uv[0] = p.uv[1].x;
				vertice[1].uv[1] = p.uv[1].y;
				vertice[2].uv[0] = p.uv[2].x;
				vertice[2].uv[1] = p.uv[2].y;
				vertice[3].uv[0] = p.uv[3].x;
				vertice[3].uv[1] = p.uv[3].y;
			}

			ResourceImageDefault * image = m_resource->getAtlasImage( mesh.texture );
			const ResourceImage::ImageFrame & frame = image->getImageFrame( 0 );

			Texture* texture = frame.texture;

			//++partCount;

			Batch batch;
			batch.begin = mesh.begin * 4;
			batch.size = mesh.size * 4;
			batch.texture[0] = texture;
			batch.material = mesh.intense ? m_materials[0] : m_materials[1];

			m_batchs.push_back( batch );
		}

		ParticleEngine::get()
			->renderParticlesCount(partCount);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::onParticleEmitterStopped()
	{
		this->end();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::restart()
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_interface->restart();
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
		if( this->isActivate() == false )
		{
			return;
		}

		//m_interface->setLeftBorder( _pos );
		m_interface->restart();
		m_interface->play();
		m_interface->update( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterTranslateWithParticle( bool _with )
	{
		if( m_interface == 0 )
		{
			return;
		}

		m_interface->setEmitterTranslateWithParticle( _with );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterRelative( bool _relative )
	{
		bool oldRelative = m_emitterRelative;
		
		m_emitterRelative = _relative;

		if( oldRelative != m_emitterRelative )
		{
			updateRelative_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setStartPosition( float _pos )
	{
		m_startPosition = _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterImage( const ConstString & _emitterImageName )
	{
		m_emitterImageName = _emitterImageName;

		if( this->isCompile() == false )
		{
			return;
		}

		this->compileEmitterImage_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::compileEmitterImage_()
	{
		ResourceImage * resourceImage = ResourceManager::get()
			->getResourceT<ResourceImage>(m_emitterImageName);

		if( resourceImage == 0 )
		{
			MENGE_LOG_ERROR( "ParticleEmitter '%s' can't compile emitter image %s"
				, m_name.c_str()
				, m_emitterImageName.c_str()
				);

			return false;
		}

		size_t alphaWidth = 0;
		size_t alphaHeight = 0;

		unsigned char * alphaMap = AlphaChannelManager::get()
			->getAlphaBuffer( m_emitterImageName, resourceImage, 0, alphaWidth, alphaHeight );

		m_interface->changeEmitterImage( alphaWidth, alphaHeight, alphaMap, 1 );

		ResourceManager::get()
			->releaseResource( resourceImage );

		AlphaChannelManager::get()
			->releaseAlphaBuffer( m_emitterImageName );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::updateRelative_()
	{
		if( m_interface == NULL )
		{
			return;
		}

		if( m_emitterRelative == false )
		{
			mt::vec2f pos;
			m_interface->getPosition( pos );

			Scene * scene = this->getScene();
			Layer * mainLayer = scene->getMainLayer();

			const mt::vec2f & layerSize = mainLayer->getSize();

			mt::vec2f new_pos;
			new_pos.x = pos.x + layerSize.x * 0.5f;
			new_pos.y = pos.y + layerSize.y * 0.5f;

			m_interface->setPosition( new_pos );
		}
		else
		{
			const mt::vec2f& pos = this->getWorldPosition();
			m_interface->setPosition( pos );

			const mt::vec2f& dir = this->getWorldDirection();
			float angle = mt::signed_angle( dir );
			m_interface->setAngle( angle );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::changeEmitterPolygon( const mt::polygon & _polygon )
	{
		if( this->isCompile() == false ) 
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

		this->setEmitterRelative( true );

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

		if( m_emitterRelative == true )
		{
			const mt::vec2f& pos = this->getWorldPosition();
			m_interface->setPosition( pos );

			const mt::vec2f& dir = this->getWorldDirection();
			float angle = mt::signed_angle( dir );
			m_interface->setAngle( angle );
		}
	}
}
