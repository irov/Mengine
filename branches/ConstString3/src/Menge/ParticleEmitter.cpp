#	include "ParticleEmitter.h" 

#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "EventManager.h"

#	include "RenderMaterial.h"
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
#	include "AlphaChannel.h"

#	include "Sprite.h"
#	include "Consts.h"

#	include "math/box2.h"
#	include "math/angle.h"
//#	include "math/triangulation.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter::ParticleEmitter()
		: m_interface(0)
		, m_resource(0)
		, m_startPosition(0.f)
		, m_emitterRelative(false)
		, m_cacheEmitterRelative(false)
		, m_centerAlign(false)
		, m_checkViewport(NULL)
		, m_positionOffset(0.f, 0.f)
		, m_emitterTranslateWithParticle(true)
		, m_vertices(NULL)
		, m_verticesCount(0)
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
		Eventable::registerEvent( EVENT_PARTICLE_EMITTER_RESTART, ("onParticleEmitterRestart"), _listener );
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_activate()
	{
		if( Node::_activate() == false )
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
	bool ParticleEmitter::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

        ////// it`s not very pretty
        if( m_emitterName.empty() == true )
        {
            return false;
        }

		m_resource = ResourceManager::get()
			->getResourceT<ResourceEmitterContainer>( m_resourcename );

		if( m_resource == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEmitter::_compile '%s' can't open resource file '%s'"
				, m_name.c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}
		
		EmitterContainerInterface * container = m_resource->getContainer();

		if( container == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEmitter::_compile '%s' can't open container file '%s'"
				, m_name.c_str()
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
		//m_interface->setEmitterTranslateWithParticle( true );

		m_interface->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );

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

		const RenderMaterialGroup * mg_intensive = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(ParticleIntensive) );

		const RenderMaterialGroup * mg_nonintensive = RenderEngine::get()
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

		m_cacheEmitterRelative = false;

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
			m_interface = NULL;
		}

		if( m_resource != 0 )
		{
			m_resource->decrementReference();
			m_resource = NULL;
		}

		delete [] m_vertices;
		m_vertices = NULL;
		m_verticesCount = 0;

		//m_images.clear();				
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		bool enabled = Application::get()
			->getParticlesEnabled();

		if( enabled == false )
		{
			return;
		}

		if( this->isPlay() == false)
		{
			return;
		}

		size_t maxParticleCount = ParticleEngine::get()
			->getMaxParticlesCount();

		this->updateParticleVertex_();

		for( TVectorBatchs::const_iterator
			it = m_batchs.begin(),
			it_end = m_batchs.end();
		it != it_end;
		++it )
		{
			const Batch & batch = *it;

			RenderEngine::get()->
				addRenderObject2D( _camera, batch.material, batch.texture, 1, &m_vertices[batch.begin], batch.size, LPT_QUAD );
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

		//TODO!???
		if( m_startPosition > 0.f )
		{
			m_interface->update( m_startPosition );
		}
		//ParticleEmitter::_update( 0.f, m_startPosition );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_restart( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		EVENTABLE_CALL(this, EVENT_PARTICLE_EMITTER_RESTART)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );

		m_interface->restart();

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
	void ParticleEmitter::_stop( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_interface->stop();

		EVENTABLE_CALL(this, EVENT_PARTICLE_EMITTER_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_end( size_t _enumerator )
	{
		EVENTABLE_CALL(this, EVENT_PARTICLE_EMITTER_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
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
	bool ParticleEmitter::getLoop() const
	{
		if( this->isCompile() == false )
		{
			return false;
		}

		return m_interface->getLoop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_interrupt( size_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			return false;
		}

		m_interface->interrupt();

		//this->callEventDeferred( EVENT_PARTICLE_EMITTER_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );

		return true;
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
	void ParticleEmitter::_update( float _current, float _timing )
	{
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

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

		m_interface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::updateParticleVertex_()
	{
		size_t partCount = 0;
				
		Node::_updateBoundingBox( m_localBoundingBox );

		ColourValue color;
		this->calcTotalColor(color);
		float a =  color.getA();

		ARGB color_argb = color.getAsARGB();

		size_t maxParticleCount = 2000;
		size_t maxMeshCount = 100;

		static TVectorParticleVerices s_particles(maxParticleCount);
		static TVectorParticleMeshes s_meshes(maxMeshCount);
		//s_particles.clear();
		//s_meshes.clear();

		//m_vertices.clear();		

		m_batchs.clear();

		EmitterRenderFlush flush;

		if( ParticleEngine::get()
			->flushEmitter( m_interface, s_meshes, s_particles, maxParticleCount, flush ) == false )
		{
			return;
		}

		if( m_verticesCount < flush.particleCount * 4 )
		{			
			m_verticesCount = flush.particleCount * 4;

			delete [] m_vertices;
			m_vertices = new Vertex2D [m_verticesCount];
		}

		const mt::mat4f& worldMatrix = this->getWorldMatrix();
		
		for( TVectorParticleMeshes::size_type
			it = 0,
			it_end = flush.meshCount;
		it != it_end;
		++it )
		{
			const ParticleMesh & mesh = s_meshes[it];

			ResourceImageDefault * image = m_resource->getAtlasImage( mesh.texture );
			RenderTextureInterface* texture = image->getTexture();

			const mt::vec4f & mesh_uv = texture->getUV();

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

				//mt::vec2f worldVertice;
				//mt::mul_v2_m3(worldVertice,p.v[0],worldMatrix);
								
				//vertice[0].pos[0] = worldVertice.x;
				//vertice[0].pos[1] = worldVertice.y;
				vertice[0].pos[0] = p.v[0].x;
				vertice[0].pos[1] = p.v[0].y;
				vertice[0].pos[2] = 0.f;
				//vertice[0].pos[3] = 1.f;

				vertice[0].color = argb;

				//mt::mul_v2_m3(worldVertice,p.v[1],worldMatrix);

				//vertice[1].pos[0] = worldVertice.x;
				//vertice[1].pos[1] = worldVertice.y;
				vertice[1].pos[0] = p.v[1].x;
				vertice[1].pos[1] = p.v[1].y;
				vertice[1].pos[2] = 0.f;
				//vertice[1].pos[3] = 1.f;
				vertice[1].color = argb;

				//mt::mul_v2_m3(worldVertice,p.v[2],worldMatrix);
				
				//vertice[2].pos[0] = worldVertice.x;
				//vertice[2].pos[1] = worldVertice.y;
				vertice[2].pos[0] = p.v[2].x;
				vertice[2].pos[1] = p.v[2].y;
				vertice[2].pos[2] = 0.f;
				//vertice[2].pos[3] = 1.f;

				vertice[2].color = argb;

				//mt::mul_v2_m3(worldVertice,p.v[3],worldMatrix);

				//vertice[3].pos[0] = worldVertice.x;
				//vertice[3].pos[1] = worldVertice.y;
				vertice[3].pos[0] = p.v[3].x;
				vertice[3].pos[1] = p.v[3].y;
				vertice[3].pos[2] = 0.f;
				//vertice[3].pos[3] = 1.f;

				vertice[3].color = argb;
				
				mt::vec2f uv[4];

				uv[0].x = mesh_uv.x + (mesh_uv.z - mesh_uv.x) * p.uv[0].x;
				uv[0].y = mesh_uv.y + (mesh_uv.w - mesh_uv.y) * p.uv[0].y;
				uv[1].x = mesh_uv.x + (mesh_uv.z - mesh_uv.x) * p.uv[1].x;
				uv[1].y = mesh_uv.y + (mesh_uv.w - mesh_uv.y) * p.uv[1].y;
				uv[2].x = mesh_uv.x + (mesh_uv.z - mesh_uv.x) * p.uv[2].x;
				uv[2].y = mesh_uv.y + (mesh_uv.w - mesh_uv.y) * p.uv[2].y;
				uv[3].x = mesh_uv.x + (mesh_uv.z - mesh_uv.x) * p.uv[3].x;
				uv[3].y = mesh_uv.y + (mesh_uv.w - mesh_uv.y) * p.uv[3].y;

				vertice[0].uv[0] = uv[0].x;
				vertice[0].uv[1] = uv[0].y;
				vertice[1].uv[0] = uv[1].x;
				vertice[1].uv[1] = uv[1].y;
				vertice[2].uv[0] = uv[2].x;
				vertice[2].uv[1] = uv[2].y;
				vertice[3].uv[0] = uv[3].x;
				vertice[3].uv[1] = uv[3].y;
			}

			++partCount;

			Batch batch;
			batch.begin = mesh.begin * 4;
			batch.size = mesh.size * 4;
			batch.texture[0] = texture;
			batch.material = mesh.intense ? m_materials[0] : m_materials[1];

			m_batchs.push_back( batch );
		}

		ParticleEngine::get()
			->renderParticlesCount( partCount );
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

		m_interface->seek(_pos);
		//m_interface->setLeftBorder( _pos );
		//m_interface->restart();
		//m_interface->play();
		//m_interface->update( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterTranslateWithParticle( bool _with )
	{
		m_emitterTranslateWithParticle = _with;
		
		if( this->isCompile() == false )
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
			this->updateRelative_();
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

		AlphaChannel * alphaMap = AlphaChannelManager::get()
			->getAlphaChannel( m_emitterImageName, resourceImage );

		 unsigned char * alphaBuffer = alphaMap->getAlphaBuffer( 0, alphaWidth, alphaHeight );

		if (m_interface->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false)
		{
			MENGE_LOG_ERROR("ParticleEmitter '%s'::setEmitterImage changeEmitterImage Error image %s"
			, m_name.c_str()
			, m_emitterImageName.c_str()
			);

			return false;
		}
				
		if( resourceImage != 0 )
		{
			resourceImage->decrementReference();
			resourceImage = 0;
		}

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
			if( m_cacheEmitterRelative == false )
			{
				m_cacheEmitterRelative = true;
				
				m_interface->getPosition( m_positionEffect );
				
				mt::vec2f relativeSize = this->getRelativeSize();

				if( relativeSize.x == 0 || relativeSize.y == 0 )
				{	
					Scene * scene = this->getScene();
					Layer * mainLayer = scene->getMainLayer();
					relativeSize = mainLayer->getSize();
				}

				const mt::mat4f & wm = this->getWorldMatrix();

				mt::vec2f new_pos = relativeSize * 0.5f + m_positionEffect;

				mt::vec2f wm_pos;
				mt::mul_v2_m4( wm_pos, new_pos, wm );

				//new_pos.x = scene_pos.x + (m_positionEffect.x * scale.x) + relativeSize.x * 0.5f;
				//new_pos.y = scene_pos.y + (m_positionEffect.y * scale.y) + relativeSize.y * 0.5f;

				//printf(" (%f %f) (%f %f) (%f %f) %s \n",  new_pos.x,new_pos.y, m_positionEffect.x,m_positionEffect.y,scene_pos.x, scene_pos.y ,m_name.c_str() );

				m_interface->setPosition( wm_pos );
			}
		}
		else
		{
			this->invalidateWorldMatrix();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::changeEmitterPolygon( const Polygon & _polygon )
	{
		if( this->isCompile() == false ) 
		{
			MENGE_LOG_ERROR("ParticleEmitter::changeEmitterPolygon '%s' invalid not compile"
				, m_name.c_str()
				);

			return;
		}

		TVectorPoints points;
		if( triangulate_polygon( _polygon, points ) == false )
		{
			MENGE_LOG_ERROR("ParticleEmitter::changeEmitterPolygon '%s' wrong polygon"
				, m_name.c_str()
				);

			return;
		}

		if( points.empty() == true )
		{
			return;
		}

		this->setEmitterRelative( true );

		if( m_interface->changeEmitterModel( points.front().buff(), points.size() / 3 ) == false)
		{
			MENGE_LOG_ERROR("ParticleEmitter::changeEmitterPolygon '%s' changeEmitterModel Error polygon"
				, m_name.c_str()
				);

			return;
		}
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
			const mt::vec3f& pos = this->getWorldPosition();
			mt::vec2f realPos = pos.to_vec2f();
			realPos.x += m_positionOffset.x;
			realPos.y += m_positionOffset.y;
			m_interface->setPosition( realPos );

			//const mt::vec2f& dir = this->getWorldDirection();
			//float angle = mt::signed_angle( dir );
			//m_interface->setAngle( angle );
		}
		else
		{
			mt::vec2f relativeSize = this->getRelativeSize();

			if( relativeSize.x == 0 || relativeSize.y == 0 )
			{	
				Scene * scene = this->getScene();
				Layer * mainLayer = scene->getMainLayer();
				relativeSize = mainLayer->getSize();
				//scale = mainLayer->getScale();
			}

			const mt::mat4f & wm = this->getWorldMatrix();
			
			mt::vec2f new_pos = relativeSize * 0.5f + m_positionEffect;

			mt::vec2f wm_pos;
			mt::mul_v2_m4( wm_pos, new_pos, wm );

			//new_pos.x = scene_pos.x + (m_positionEffect.x * scale.x) + relativeSize.x * 0.5f;
			//new_pos.y = scene_pos.y + (m_positionEffect.y * scale.y) + relativeSize.y * 0.5f;

			//printf(" (%f %f) (%f %f) (%f %f) %s \n",  new_pos.x,new_pos.y, m_positionEffect.x,m_positionEffect.y,scene_pos.x, scene_pos.y ,m_name.c_str() );

			m_interface->setPosition( wm_pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setPositionOffset( const mt::vec2f& _offset )
	{
		m_positionOffset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ParticleEmitter::getPositionOffset() const
	{
		return m_positionOffset;
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getDuration() const
	{
		return m_interface->getDuration();
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getLeftBorder() const
	{
		return m_interface->getLeftBorder();
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getRightBorder() const
	{
		return m_interface->getRightBorder();
	}
	/////////////////////////////////////////////////////////////////////////
	const ConstString& ParticleEmitter::getEmitterName() const
	{
		return m_emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::box2f ParticleEmitter::getEmitterBoundingBox() const
	{
		mt::box2f _box;
		m_interface->getBoundingBox(_box);
		return _box;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f ParticleEmitter::getEmitterPosition() const
	{
		mt::vec2f pos;
		m_interface->getPosition(pos);
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ParticleEmitter::getRelativeSize() const
	{
		EmitterContainerInterface * container = m_resource->getContainer();
		const EmitterContainerMetaData & meta = container->getMetaData();
		return meta.size;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setRandomMode( bool _randomMode )
	{
		if( m_interface == 0 )
		{
			return;
		}

		m_interface->setRandomMode( _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::getRandomMode() const
	{
		return m_interface->getRandomMode();
	}
	//////////////////////////////////////////////////////////////////////////
}
