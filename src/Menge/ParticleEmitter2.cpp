#	include "ParticleEmitter2.h" 

#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/ApplicationInterface.h"

#	include "Player.h"

#	include "Kernel/Scene.h"
#	include "Kernel/Layer.h"

#   include "Interface/ParticleSystemInterface.h"

#	include "ResourceImageDefault.h"
#	include "ResourceHIT.h"

#   include "Interface/ResourceInterface.h"

#	include "Sprite.h"
#	include "Consts.h"

#	include "math/box2.h"
#	include "math/angle.h"
//#	include "math/triangulation.h"

#   include "pybind/pybind.hpp"

#	include "Core/RenderUtil.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter2::ParticleEmitter2()
		: m_emitter(nullptr)
        , m_randomMode(false)
		, m_vertices(nullptr)
		, m_vertexCount(0)
		, m_indicies(nullptr)
		, m_indexCount(0)
        , m_emitterRelative(false)
        , m_emitterTranslateWithParticle(true)
	{
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

		//if( m_emitterRelative == true )
		//{
		//	const mt::vec3f & wm_pos = this->getWorldPosition();

		//	m_emitter->setPosition( wm_pos );
		//}
        
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

		ParticleEmitterInterfacePtr emitter = m_resourceParticle->createEmitter();

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

		uint32_t polygon_count = m_polygon.num_points();

        if( polygon_count != 0 )
        {
			if( this->compilePolygon_( emitter ) == false )
            {
                return false;
            }
        }

		if( m_emitterRelative == true )
		{
			emitter->setPositionProvider( this );
		}
		else
		{
			emitter->setPositionProvider( nullptr );
		}

		m_emitter = emitter;

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_release()
	{
        m_emitter = nullptr;

        m_resourceParticle.release();

		Helper::freeMemory( m_vertices );
		m_vertices = nullptr;
		m_vertexCount = 0;

		Helper::freeMemory( m_indicies );
		m_indicies = nullptr;
		m_indexCount = 0;		
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
	void ParticleEmitter2::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		Node::_update( _current, _timing );

		bool enabled = APPLICATION_SERVICE(m_serviceProvider)
			->getParticleEnable();

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

		float scretch = this->getStretch();

		float totalTiming = timing / scretch;

		bool stop;
		m_emitter->update( totalTiming, stop );

		if( stop == true )
		{
			this->end();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_render( const RenderObjectState * _state )
	{
		Node::_render( _state );

		bool enabled = APPLICATION_SERVICE( m_serviceProvider )
			->getParticleEnable();

		if( enabled == false )
		{
			return;
		}

		if( this->isPlay() == false )
		{
			return;
		}

		ParticleEmitterRenderFlush flush;
		if( m_emitter->prepareParticles( flush ) == false )
		{
			return;
		}

		if( m_vertexCount < flush.vertexCount )
		{
			m_vertexCount = flush.vertexCount;

			m_vertices = Helper::reallocateMemory<RenderVertex2D>( m_vertices, m_vertexCount );
		}

		if( m_indexCount < flush.indexCount )
		{
			m_indexCount = flush.indexCount;

			m_indicies = Helper::reallocateMemory<RenderIndices>( m_indicies, m_indexCount );
		}

		ParticleMesh meshes[MENGINE_PARTICLE_MAX_MESH];

		if( m_emitter->flushParticles( meshes, MENGINE_PARTICLE_MAX_MESH, m_vertices, m_indicies, flush ) == false )
		{
			return;
		}

		this->updateVertexColor_( m_vertices, flush.vertexCount );

		if( m_emitterRelative == false )
		{
			this->updateVertexWM_( m_vertices, flush.vertexCount );
		}

		const mt::box2f & bb = this->getBoundingBox();
		
		for( uint32_t
			it_mesh = 0,
			it_mesh_end = flush.meshCount;
		it_mesh != it_mesh_end;
		++it_mesh )
		{
			const ParticleMesh & mesh = meshes[it_mesh];

			RenderTextureInterfacePtr textures[MENGE_MAX_TEXTURE_STAGES];

			for( uint32_t i = 0; i != mesh.textures; ++i )
			{
				//int textureId = mesh.texture[i];

				//ResourceImage * image = m_resourceParticle->getAtlasImageResource( textureId );
				//TODO
				ResourceImage * image = m_resourceParticle->getAtlasImageResource( 0 );

				if( image == nullptr )
				{
					return;
				}

				const RenderTextureInterfacePtr & texture = image->getTexture();

				textures[i] = texture;
			}

			const RenderStage * stage = PARTICLE_SYSTEM2( m_serviceProvider )
				->getMaterialStage( mesh.material );

			RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( m_serviceProvider )
				->getMaterial2( stage, PT_TRIANGLELIST, mesh.textures, textures );

			RENDER_SERVICE( m_serviceProvider )
				->addRenderObject( _state, material, m_vertices + mesh.vertexOffset, mesh.vertexCount, m_indicies + mesh.indexOffset, mesh.indexCount, &bb, false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::updateVertexColor_( RenderVertex2D * _vertices, uint32_t _verticesCount )
	{ 
		ColourValue color;
		this->calcTotalColor( color );

		if( color.isIdentity() == true )
		{
			return;
		}

		ColourValue_ARGB color_argb = color.getAsARGB();

		for( uint32_t
			it = 0,
			it_end = _verticesCount;
		it != it_end;
		++it )
		{
			RenderVertex2D & p = _vertices[it];
			
			if( p.color == 0xFFFFFFFF )
			{
				p.color = color_argb;
			}
			else
			{
				ColourValue cv( p.color );
				cv *= color;
				p.color = cv.getAsARGB();								 
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter2::updateVertexWM_( RenderVertex2D * _vertices, uint32_t _verticesCount )
    {        
        if( this->isIdentityWorldMatrix() == true )
        {
            return;
        }

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t
			it = 0,
			it_end = _verticesCount;
		it != it_end;
		++it )
		{
			mt::vec3f & pos = _vertices[it].pos;

			mt::vec3f wm_pos;
			mt::mul_v3_m4( wm_pos, pos, wm );

			_vertices[it].pos = wm_pos;
		}
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
	void ParticleEmitter2::setEmitterTranslateWithParticle( bool _translateWithParticle )
	{
		m_emitterTranslateWithParticle = _translateWithParticle;
		
		if( this->isCompile() == false )
		{
			return;
		}

		m_emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterRelative( bool _relative )
	{
        m_emitterRelative = _relative;

		if( this->isCompile() == false )
		{
			return;
		}

		if( m_emitterRelative == true )
		{
			m_emitter->setPositionProvider( this );
		}
		else
		{ 
			m_emitter->setPositionProvider( nullptr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::changeEmitterImage( const ConstString & _emitterImageName )
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

		m_polygon.correct();

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
		uint32_t n = m_polygon.num_points();

        if( n == 0 )
        {
            return true;
        }

		TVectorPoints points;
		if( m_polygon.triangulate( points ) == false )
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
		const mt::box2f & bb = m_emitter->getBoundingBox();

        _boundingBox = bb;
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
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterRandomMode( bool _randomMode )
	{
        m_randomMode = _randomMode;

        if( this->isCompile() == false )
        {
            return;
        }

		m_emitter->setRandomMode( _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::getEmitterRandomMode() const
	{
        return m_randomMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::onProviderEmitterPosition( mt::vec3f & _position )
	{ 
		mt::vec3f position;
		mt::vec3f origin;
		mt::vec3f coordinate;
		mt::vec3f scale; 
		mt::vec3f orientation;
		this->getTransformation( position, origin, coordinate, scale, orientation );

		mt::mat4f wm;
		this->calcWorldMatrix( wm, position + mt::vec3f( 1024.f, 1024.f, 0.f ), origin, coordinate, scale, orientation );
		
		_position = wm.v3.to_vec3f();
	}
}
