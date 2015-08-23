#	include "ParticleEmitter2.h" 

#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/ApplicationInterface.h"

#	include "Player.h"

#	include "Kernel/Scene.h"
#	include "Kernel/Layer.h"

#   include "Interface/ParticleSystemInterface.h"

#	include "ResourceEmitterContainer.h"
#	include "ResourceImageDefault.h"
#	include "ResourceHIT.h"

#   include "Interface/ResourceInterface.h"

#   include "Interface/AlphaChannelInterface.h"

#	include "Sprite.h"
#	include "Consts.h"

#	include "math/box2.h"
#	include "math/angle.h"
//#	include "math/triangulation.h"

#   include "pybind/pybind.hpp"

#	include "Core/RenderUtil.h"
#	include "Core/Memory.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter2::ParticleEmitter2()
		: m_emitter(nullptr)
		, m_startPosition(0.f)
        , m_randomMode(false)
		, m_vertices(nullptr)
		, m_verticesCount(0)
        , m_emitterRelative(false)
        , m_emitterPosition(0.f, 0.f, 0.f)
        , m_emitterTranslateWithParticle(true)
		, m_invalidateMaterial(true)
	{
		for( uint32_t i = 0; i != MENGINE_PARTICLE_MAX_ATLAS_TEXTURE; ++i )
		{
			m_materials[i * 2 + 0] = nullptr;
			m_materials[i * 2 + 1] = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter2::~ParticleEmitter2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_PARTICLE_EMITTER_END, ("onParticleEmitterEnd"), _listener );
		this->registerEvent( EVENT_PARTICLE_EMITTER_RESTART, ("onParticleEmitterRestart"), _listener );
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_emitterRelative == true )
		{
			const mt::vec3f & wm_pos = this->getWorldPosition();

			m_emitter->setPosition( wm_pos );
		}
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_deactivate()
	{
		this->stop();

		Node::_deactivate();		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_compile()
	{      		
		if( m_resourceParticle == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter2::_compile '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceParticle.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter2::_compile '%s' resource '%s' not compile"
                , m_name.c_str()
                , m_resourceParticle->getName().c_str()
                );

            return false;
        }

		const ParticleEmitterContainerInterface2Ptr & container = m_resourceParticle->getContainer();

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter2::_compile '%s' can't open container file '%s'"
				, m_name.c_str()
				, m_resourceParticle->getName().c_str() 
				);

			return false;
		}

		ParticleEmitterInterfacePtr emitter = container->createEmitter();

		if( emitter == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter2::_compile '%s' can't create emitter source '%s'"
				, m_name.c_str()
				, m_resourceParticle->getName().c_str()
				);

			return false;
		}

		emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );

		if( m_emitterImageName.empty() == false )
		{
			if( this->compileEmitterImage_( emitter ) == false )
			{
				return false;
			}
		}

		emitter->setRandomMode( m_randomMode );

        bool loop = this->getLoop();
		emitter->setLoop( loop );

        uint32_t polygon_count = boost::geometry::num_points( m_polygon );

        if( polygon_count != 0 )
        {
			if( this->compilePolygon_( emitter ) == false )
            {
                return false;
            }
        }

		if( emitter->isBackground() == false )
		{
			emitter->setPosition( m_emitterPosition );
		}

		m_emitter = emitter;

		this->invalidateMaterial_();

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_release()
	{
        m_emitter = nullptr;

        m_resourceParticle.release();

		Helper::freeMemory( m_vertices );
		m_vertices = nullptr;

		m_verticesCount = 0;

        m_batchs.clear();
		
		for( uint32_t i = 0; i != MENGINE_PARTICLE_MAX_ATLAS_TEXTURE; ++i )
		{
			m_materials[i * 2 + 0] = nullptr;
			m_materials[i * 2 + 1] = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{		
		Node::_render( _viewport, _camera );

		bool enabled = APPLICATION_SERVICE(m_serviceProvider)
			->getParticlesEnabled();

		if( enabled == false )
		{
			return;
		}

		if( this->isPlay() == false)
		{
			return;
		}

        if( m_vertices == nullptr )
        {
            return;
        }
		
		for( TVectorBatchs::const_iterator
			it = m_batchs.begin(),
			it_end = m_batchs.end();
		it != it_end;
		++it )
		{
			const Batch & batch = *it;

            RenderVertex2D * batch_vertices = m_vertices + batch.begin;

            if( batch.begin + batch.size > m_verticesCount )
            {
                return;
            }

            RENDER_SERVICE(m_serviceProvider)
				->addRenderQuad( _viewport, _camera, batch.material, batch_vertices, batch.size, &batch.bb, false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_play( float _time )
	{
        (void)_time;

		if( this->isActivate() == false )
		{
			return false;
		}

		m_emitter->play();

		//TODO!???
		if( m_startPosition > 0.f )
		{
			bool stop;
			m_emitter->update( m_startPosition, stop );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_restart( float _time, uint32_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_RESTART)( this, _enumerator, false );

		m_emitter->restart();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;

		if( this->isActivate() == false )
		{
			return;
		}

		m_emitter->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_resume( uint32_t _enumerator )
	{
		(void)_enumerator;

		if( this->isActivate() == false )
		{
			return;
		}

		m_emitter->resume();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_stop( uint32_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_emitter->stop();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_END)( this, _enumerator, false );		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_end( uint32_t _enumerator )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_END)( this, _enumerator, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_setLoop( bool _value )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		m_emitter->setLoop( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		if( this->isCompile() == false )
		{
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::_interrupt '%s' can't compile"
                , m_name.c_str()
                );

			return false;
		}

		m_emitter->interrupt();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setLeftBorder( float _leftBorder )
	{
		if( this->isCompile() == false )
		{
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::setLeftBorder '%s' can't compile"
                , m_name.c_str()
                );

			return;
		}

		return m_emitter->setLeftBorder( _leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		Node::_update( _current, _timing );

		bool enabled = APPLICATION_SERVICE(m_serviceProvider)
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

		float speedFactor = this->getAnimationSpeedFactor();
		float timing = _timing * speedFactor;

		float scretch = this->getScretch();

		float totalTiming = timing / scretch;

		bool stop;
		m_emitter->update( totalTiming, stop );
		
		this->updateParticleVertex_();

		if( stop == true )
		{
			this->end();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::updateParticleVertex_()
	{
		uint32_t partCount = 0;

		m_batchs.clear();

		uint32_t meshes_limit;
		ParticleMesh * meshes_cache = PARTICLE_SERVICE2( m_serviceProvider )
			->getParticleMeshesCache( meshes_limit );

		uint32_t vertices_limit;
		ParticleVertices * vertices_cache = PARTICLE_SERVICE2( m_serviceProvider )
			->getParticleVerticesCache( vertices_limit );

		ParticleEmitterRenderFlush flush;
		if( m_emitter->flushParticles( meshes_cache, meshes_limit, vertices_cache, vertices_limit, flush ) == false )
		{
			return false;
		}

		if( m_verticesCount < flush.particleCount * 4 )
		{			
			m_verticesCount = flush.particleCount * 4;

			m_vertices = Helper::reallocateMemory<RenderVertex2D>( m_vertices, m_verticesCount );
		}

		ColourValue color;
		this->calcTotalColor(color);

		ColourValue_ARGB color_argb = color.getAsARGB();
		
		for( uint32_t
			it_mesh = 0,
			it_mesh_end = flush.meshCount;
		it_mesh != it_mesh_end;
		++it_mesh )
		{
			const ParticleMesh & mesh = meshes_cache[it_mesh];

			ResourceImage * image = m_resourceParticle->getAtlasImageResource( mesh.texture );
						
			const mt::uv4f & mesh_uv_image = image->getUVImage();
			const mt::uv4f & mesh_uv_alpha = image->getUVAlpha();

			bool mesh_uv_image_identity = mt::uv4_identity( mesh_uv_image );
			bool mesh_uv_alpha_identity = mt::uv4_identity( mesh_uv_alpha );

			for( uint32_t
				it = mesh.begin,
				it_end = mesh.begin + mesh.size;
			it != it_end;
			++it )
			{
				const ParticleVertices & p = vertices_cache[it];

				uint32_t argb = 0xFFFFFFFF;

#	ifdef MENGE_RENDER_TEXTURE_RGBA
				uint32_t a = (p.color >> 24) & 0xFF;
				uint32_t b = (p.color >> 16) & 0xFF;
				uint32_t g = (p.color >> 8) & 0xFF;
				uint32_t r = (p.color >> 0) & 0xFF;

				ColourValue_ARGB p_color = (a << 24) | (r << 16) | (g << 8) | (b << 0);
#	else
				ColourValue_ARGB p_color = p.color;
#	endif

				if( color.isIdentity() == true )
				{
					argb = p_color;
				}
				else if( p_color == 0xFFFFFFFF )
				{
					argb = color_argb;
				}
				else
				{
					ColourValue cv(p_color);
					cv *= color;
					argb = cv.getAsARGB();
				}

				RenderVertex2D * vertice = &m_vertices[it * 4];

				mt::uv4f uv;

				if( mesh_uv_image_identity == false )
				{
					mt::multiply_tetragon_uv4_vp( uv, mesh_uv_image, p.uv );
				}
				else
				{
					uv.p0 = p.uv[0];
					uv.p1 = p.uv[1];
					uv.p2 = p.uv[2];
					uv.p3 = p.uv[3];
				}

				mt::uv4f uv2;

				if( mesh_uv_alpha_identity == false )
				{
					mt::multiply_tetragon_uv4_vp( uv2, mesh_uv_alpha, p.uv );
				}
				else
				{
					uv2.p0 = p.uv[0];
					uv2.p1 = p.uv[1];
					uv2.p2 = p.uv[2];
					uv2.p3 = p.uv[3];
				}

				for( size_t i = 0; i != 4; ++i )
				{
					const mt::vec3f & wm_pos = p.v[i];

					vertice[i].pos = wm_pos;
					vertice[i].color = argb;
					vertice[i].uv = uv[i];
					vertice[i].uv2 = uv2[i];
				}
			}

			++partCount;

			Batch batch;
			batch.begin = mesh.begin * 4;
			batch.size = mesh.size * 4;

			uint32_t materialIndex = mesh.texture * 2 + (mesh.intense ? 0 : 1);
			
			batch.material = this->getMaterial( materialIndex );

			m_batchs.push_back( batch );
		}

        if( m_emitterRelative == false )
        {
            this->updateVertexWM_();
        }

		this->updateBB_();

		PARTICLE_SERVICE2(m_serviceProvider)
			->renderParticlesCount( partCount );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter2::updateVertexWM_()
    {        
        if( this->isIdentityWorldMatrix() == true )
        {
            return;
        }

		const mt::mat4f & wm = this->getWorldMatrix();

        for( TVectorBatchs::const_iterator
            it = m_batchs.begin(),
            it_end = m_batchs.end();
        it != it_end;
        ++it )
        {
            const Batch & batch = *it;

            for( uint32_t 
                index = batch.begin, 
                index_end = batch.begin + batch.size; 
            index != index_end; 
            ++index )
            {
                mt::vec3f & pos = m_vertices[index].pos;

                mt::vec3f wm_pos;
                mt::mul_v3_m4( wm_pos, pos, wm);

                pos = wm_pos;
            }			
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::updateBB_()
	{
		for( TVectorBatchs::iterator
			it = m_batchs.begin(),
			it_end = m_batchs.end();
		it != it_end;
		++it )
		{
			Batch & batch = *it;

			Helper::makeRenderBoundingBox( batch.bb, m_vertices + batch.begin, batch.size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::invalidateMaterial_()
	{
		m_invalidateMaterial = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::updateMaterial_()
	{
		m_invalidateMaterial = false;

		uint32_t imageCount = m_resourceParticle->getAtlasImageCount();

		if( imageCount > MENGINE_PARTICLE_MAX_ATLAS_TEXTURE )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::updateMaterial_ %s particle resource %s max atlas texture %d"
				, this->getName().c_str()
				, m_resourceParticle->getName().c_str()
				, MENGINE_PARTICLE_MAX_ATLAS_TEXTURE
				);

			return;
		}

		for( uint32_t i = 0; i != imageCount; ++i )
		{
			ResourceImage * image = m_resourceParticle->getAtlasImageResource( i );

			RenderMaterialInterfacePtr mg_intensive = Helper::makeImageMaterial( m_serviceProvider, image, ConstString::none(), EMB_ADD, false, false );
			RenderMaterialInterfacePtr mg_nonintensive = Helper::makeImageMaterial( m_serviceProvider, image, ConstString::none(), EMB_NORMAL, false, false );

			m_materials[i*2 + 0] = mg_intensive;
			m_materials[i*2 + 1] = mg_nonintensive;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::restart()
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_emitter->restart();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setResourceParticle( ResourceParticle * _resourceParticle )
	{
		if( m_resourceParticle == _resourceParticle )
		{
			return;
		}

		m_resourceParticle = _resourceParticle;

		this->recompile();
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceParticle * ParticleEmitter2::getResourceParticle() const
    {
        return m_resourceParticle;
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::playFromPosition( float _pos )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_emitter->seek( _pos );
		//m_interface->setLeftBorder( _pos );
		//m_interface->restart();
		//m_interface->play();
		//m_interface->update( _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterTranslateWithParticle( bool _with )
	{
		m_emitterTranslateWithParticle = _with;
		
		if( this->isCompile() == false )
		{
			return;
		}

		m_emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterPosition( const mt::vec3f & _position )
	{
		m_emitterPosition = _position;

		if( this->isCompile() == false )
		{
			return;
		}

		if( m_emitter->isBackground() == false )
		{
			m_emitter->setPosition( m_emitterPosition );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterRelative( bool _relative )
	{
        m_emitterRelative = _relative;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setStartPosition( float _pos )
	{
		m_startPosition = _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterImage( const ConstString & _emitterImageName )
	{
		m_emitterImageName = _emitterImageName;

        //this->removeEmitterPolygon();

		if( this->isCompile() == false )
		{
			return;
		}

		this->compileEmitterImage_( m_emitter );
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter2::removeEmitterImage()
    {
        m_emitterImageName.clear();

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->changeEmitterImage( 0, 0, 0, 1 );
    }
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::compileEmitterImage_( const ParticleEmitterInterfacePtr & _emitter )
	{
		ResourceHIT * resourceHIT = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceHIT *>(m_emitterImageName);

		if( resourceHIT == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter '%s' can't compile emitter hit %s"
				, m_name.c_str()
				, m_emitterImageName.c_str()
				);

			return false;
		}

		uint32_t alphaWidth = resourceHIT->getWidth();
		uint32_t alphaHeight = resourceHIT->getHeight();

        unsigned char * alphaBuffer = resourceHIT->getBuffer();
        
		if( _emitter->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::setEmitterImage %s changeEmitterImage Error image %s"
                , m_name.c_str()
                , m_emitterImageName.c_str()
                );

            resourceHIT->decrementReference();

			return false;
		}
				
		resourceHIT->decrementReference();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::changeEmitterPolygon( const Polygon & _polygon )
	{
        m_polygon = _polygon;

        boost::geometry::correct( m_polygon );

        if( this->isCompile() == false ) 
		{
			return true;
		}

        if( this->compilePolygon_( m_emitter ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::compilePolygon_( const ParticleEmitterInterfacePtr & _emitter )
    {
        uint32_t n = boost::geometry::num_points( m_polygon );

        if( n == 0 )
        {
            return true;
        }

		TVectorPoints points;
		if( triangulate_polygon( m_polygon, points ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::changeEmitterPolygon '%s' wrong polygon"
				, m_name.c_str()
				);

			return false;
		}

		if( points.empty() == true )
		{
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::changeEmitterPolygon '%s' empty points"
                , m_name.c_str()
                );

			return false;
		}
        
        float * triangles_ptr = points.front().buff();
        uint32_t triangles_size = points.size() / 3;

		if( _emitter->changeEmitterModel( triangles_ptr, triangles_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::changeEmitterPolygon '%s' changeEmitterModel Error polygon"
				, m_name.c_str()
				);

			return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter2::removeEmitterPolygon()
    {
        m_polygon = Polygon();
        
        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->changeEmitterModel( nullptr, 0 );
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_updateBoundingBox( mt::box2f& _boundingBox ) const
	{
        (void)_boundingBox;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_invalidateWorldMatrix()
	{
        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterRelative == true )
        {
            const mt::vec3f & wm_pos = this->getWorldPosition();

            m_emitter->setPosition( wm_pos );
        }
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter2::getDuration() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::getDuration '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float duration = m_emitter->getDuration();

		return duration;
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter2::getLeftBorder() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::getLeftBorder '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float leftBoard = m_emitter->getLeftBorder();

		return leftBoard;
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter2::getRightBorder() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::getRightBorder '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float rightBoard = m_emitter->getRightBorder();

		return rightBoard;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::box2f ParticleEmitter2::getEmitterBoundingBox() const
	{
		mt::box2f box;

        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::getEmitterBoundingBox '%s' can't compile"
                , m_name.c_str()
                );

            return box;
        }

		m_emitter->getBoundingBox( box );

		return box;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f ParticleEmitter2::getEmitterPosition() const
	{
		mt::vec3f pos;

        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::getEmitterPosition '%s' can't compile"
                , m_name.c_str()
                );

            return pos;
        }

		m_emitter->getPosition( pos );

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setRandomMode( bool _randomMode )
	{
        m_randomMode = _randomMode;

        if( this->isCompile() == false )
        {
            return;
        }

		m_emitter->setRandomMode( _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::getRandomMode() const
	{
        return m_randomMode;
	}
}
