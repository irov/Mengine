#	include "ParticleEmitter.h" 

#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/ApplicationInterface.h"

#	include "Camera2D.h"

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

namespace	Menge
{
    //////////////////////////////////////////////////////////////////////////
    static const size_t maxParticleCount = 2000;
    static const size_t maxMeshCount = 100;
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter::ParticleEmitter()
		: m_interface(nullptr)
		, m_startPosition(0.f)
        , m_randomMode(false)
		, m_vertices(nullptr)
		, m_verticesCount(0)
        , m_emitterRelative(false)
        , m_emitterPosition(0.f, 0.f, 0.f)
        , m_emitterTranslateWithParticle(true)
        , m_emitterChangeRendering(true)
        , m_emitterChangeRenderingExtra(true)
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

		this->registerEvent( EVENT_PARTICLE_EMITTER_END, ("onParticleEmitterEnd"), _listener );
		this->registerEvent( EVENT_PARTICLE_EMITTER_RESTART, ("onParticleEmitterRestart"), _listener );
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

        m_emitterChangeRendering = true;
        m_emitterChangeRenderingExtra = true;
        
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
        ////// it`s not very pretty
        if( m_emitterName.empty() == true )
        {
            return false;
        }
        		
		if( m_resourceEmitterContainer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::_compile '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceEmitterContainer.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::_compile '%s' resource '%s' not compile"
                , m_name.c_str()
                , m_resourceEmitterContainer->getName().c_str()
                );

            return false;
        }
		const ParticleEmitterContainerInterfacePtr & container = m_resourceEmitterContainer->getContainer();

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::_compile '%s' can't open container file '%s'"
				, m_name.c_str()
				, m_resourceEmitterContainer->getName().c_str() 
				);

			return false;
		}

		m_interface = container->createEmitter( m_emitterName );

		if( m_interface == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter '%s' can't create emitter source '%s' - '%s'"
				, m_name.c_str()
				, m_resourceEmitterContainer->getName().c_str()
				, m_emitterName.c_str() 
				);

			return false;
		}

		m_interface->setListener( this );

		m_interface->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );

		const RenderMaterialGroup * mg_intensive = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterialGroup( CONST_STRING(m_serviceProvider, ParticleIntensive) );

		const RenderMaterialGroup * mg_nonintensive = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterialGroup( CONST_STRING(m_serviceProvider, ParticleBlend) );

		m_materials[0] = mg_intensive->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_materials[1] = mg_nonintensive->getMaterial( TAM_CLAMP, TAM_CLAMP );

		if( m_emitterImageName.empty() == false )
		{
			if( this->compileEmitterImage_() == false )
			{
				return false;
			}
		}

        m_interface->setRandomMode( m_randomMode );

        bool loop = this->getLoop();
        m_interface->setLoop( loop );

        size_t polygon_count = boost::geometry::num_points( m_polygon );

        if( polygon_count != 0 )
        {
            if( this->compilePolygon_() == false )
            {
                return false;
            }
        }

		if( m_emitterRelative == true )
		{
			m_interface->setPosition( m_emitterPosition );
		}
        
		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_release()
	{
        if( m_interface != nullptr )
        {
            const ParticleEmitterContainerInterfacePtr & container = 
                m_resourceEmitterContainer->getContainer();

            container->releaseEmitter( m_interface );
            m_interface = nullptr;
        }

        m_resourceEmitterContainer.release();

		delete [] m_vertices;        
		m_vertices = nullptr;

		m_verticesCount = 0;

        m_batchs.clear();

		//m_images.clear();				
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

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

            RENDER_SERVICE(m_serviceProvider)->
                addRenderQuad( _camera, batch.material, batch.texture, 1, batch_vertices, batch.size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_play( float _time )
	{
        (void)_time;

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

        m_emitterChangeRenderingExtra = true;
		//ParticleEmitter::_update( 0.f, m_startPosition );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_restart( float _time, size_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_RESTART)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );

		m_interface->restart();

		m_emitterChangeRenderingExtra = true;

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

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_end( size_t _enumerator )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PARTICLE_EMITTER_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_setLoop( bool _value )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		m_interface->setLoop( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::_interrupt( size_t _enumerator )
	{
        (void)_enumerator;

		if( this->isCompile() == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::_interrupt '%s' can't compile"
                , m_name.c_str()
                );

			return false;
		}

		m_interface->interrupt();

		//this->callEventDeferred( EVENT_PARTICLE_EMITTER_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setLeftBorder( float _leftBorder )
	{
		if( this->isCompile() == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::setLeftBorder '%s' can't compile"
                , m_name.c_str()
                );

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

		m_emitterChangeRendering = m_interface->update( _timing );

        if( this->updateParticleVertex_() == false )
        {
            return;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::updateParticleVertex_()
	{
		size_t partCount = 0;
				
		Node::_updateBoundingBox( m_localBoundingBox );

		ColourValue color;
		this->calcTotalColor(color);

		ARGB color_argb = color.getAsARGB();

        static ParticleVertices s_particles[maxParticleCount];
		static ParticleMesh s_meshes[maxMeshCount];
		//s_particles.clear();
		//s_meshes.clear();

		//m_vertices.clear();		

		m_batchs.clear();

        //const mt::mat4f & viewMatrix = _camera->getViewMatrix();

        //mt::vec3f Eye(0,0,-840.f);
        //mt::vec3f At(0,0,0);        
        //mt::vec3f Up(0,1,0);

        //mt::vec3f zaxis = mt::norm_v3( At - Eye );
        //mt::vec3f xaxis = mt::norm_v3( mt::cross_v3_v3( Up, zaxis ) );
        //mt::vec3f yaxis = mt::cross_v3_v3( zaxis, xaxis );

        mt::mat4f viewMatrix;
        mt::ident_m4(viewMatrix);
        //viewMatrix.v0 = mt::vec4f(xaxis.x, yaxis.x, zaxis.x, 0);
        //viewMatrix.v1 = mt::vec4f(xaxis.y, yaxis.y, zaxis.y, 0);
        //viewMatrix.v2 = mt::vec4f(xaxis.z, yaxis.z, zaxis.z, 0);
        //viewMatrix.v3 = mt::vec4f(-mt::dot_v3_v3( xaxis, Eye ), -mt::dot_v3_v3( yaxis, Eye ),  -mt::dot_v3_v3(zaxis, Eye), 1);

		ParticleEmitterRenderFlush flush;

		if( PARTICLE_SERVICE(m_serviceProvider)
			->flushEmitter( viewMatrix, m_interface, s_meshes, s_particles, maxParticleCount, flush ) == false )
		{
			return false;
		}

		if( m_verticesCount < flush.particleCount * 4 )
		{			
			m_verticesCount = flush.particleCount * 4;

			delete [] m_vertices;
			m_vertices = new RenderVertex2D [m_verticesCount];
		}

		//const mt::mat4f & wm = this->getWorldMatrix();

        //mt::vec2f pos;
        //m_interface->getPosition( pos );
		
		for( size_t
			it = 0,
			it_end = flush.meshCount;
		it != it_end;
		++it )
		{
			const ParticleMesh & mesh = s_meshes[it];

			const RenderTextureInterfacePtr & texture = m_resourceEmitterContainer->getAtlasTexture( mesh.texture );

			const mt::vec4f & mesh_uv = texture->getUV();

			for( size_t
				it = mesh.begin,
				it_end = mesh.begin + mesh.size;
			it != it_end;
			++it )
			{
				const ParticleVertices & p = s_particles[it];

				uint32 argb;

				if( color.isIdentity() == true )
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

				RenderVertex2D * vertice = &m_vertices[it * 4];

                const mt::vec3f & wm_pos0 = p.v[0];

                vertice[0].pos[0] = wm_pos0.x;
                vertice[0].pos[1] = wm_pos0.y;
				vertice[0].pos[2] = wm_pos0.z;


				vertice[0].color = argb;

                const mt::vec3f & wm_pos1 = p.v[1];

                vertice[1].pos[0] = wm_pos1.x;
                vertice[1].pos[1] = wm_pos1.y;
				vertice[1].pos[2] = wm_pos1.z;

                vertice[1].color = argb;
		
                const mt::vec3f & wm_pos2 = p.v[2];

				vertice[2].pos[0] = wm_pos2.x;
				vertice[2].pos[1] = wm_pos2.y;
				vertice[2].pos[2] = wm_pos2.z;

				vertice[2].color = argb;

                const mt::vec3f & wm_pos3 = p.v[3];

                vertice[3].pos[0] = wm_pos3.x;
                vertice[3].pos[1] = wm_pos3.y;
				vertice[3].pos[2] = wm_pos3.z;

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

                vertice[0].uv2[0] = 0.f;
                vertice[0].uv2[1] = 0.f;
                vertice[1].uv2[0] = 0.f;
                vertice[1].uv2[1] = 0.f;
                vertice[2].uv2[0] = 0.f;
                vertice[2].uv2[1] = 0.f;
                vertice[3].uv2[0] = 0.f;
                vertice[3].uv2[1] = 0.f;
			}

			++partCount;

			Batch batch;
			batch.begin = mesh.begin * 4;
			batch.size = mesh.size * 4;
			batch.texture[0] = texture;
			batch.material = mesh.intense ? m_materials[0] : m_materials[1];

			m_batchs.push_back( batch );
		}

        if( m_emitterRelative == false )
        {
            this->updateVertexWM_();
        }

		PARTICLE_SERVICE(m_serviceProvider)
			->renderParticlesCount( partCount );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter::updateVertexWM_()
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        if( mt::is_ident_m34( wm ) == true )
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

            for( size_t 
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
	void ParticleEmitter::setResourceEmitterContainer( ResourceEmitterContainer * _resourceEmitterContainer )
	{
		if( m_resourceEmitterContainer == _resourceEmitterContainer )
		{
			return;
		}

		m_resourceEmitterContainer = _resourceEmitterContainer;

		this->recompile();
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceEmitterContainer * ParticleEmitter::getResourceEmitterContainer() const
    {
        return m_resourceEmitterContainer;
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitter( const ConstString& _emitterName )
	{
		if( m_emitterName == _emitterName )
		{
			return;
		}

		m_emitterName = _emitterName;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::playFromPosition( float _pos )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		m_interface->seek( _pos );
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

		m_interface->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterPosition( const mt::vec3f & _position )
	{
		m_emitterPosition = _position;

		if( this->isCompile() == false )
		{
			return;
		}

		m_interface->setPosition( m_emitterPosition );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setEmitterRelative( bool _relative )
	{
        m_emitterRelative = _relative;

		if( this->isCompile() == false )
		{
			return;
		}

		m_interface->setPosition( m_emitterPosition );
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

        //this->removeEmitterPolygon();

		if( this->isCompile() == false )
		{
			return;
		}

		this->compileEmitterImage_();
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter::removeEmitterImage()
    {
        m_emitterImageName.clear();

        if( this->isCompile() == false )
        {
            return;
        }

        m_interface->changeEmitterImage( 0, 0, 0, 1 );
    }
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::compileEmitterImage_()
	{
		ResourceHIT * resourceHIT = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceHIT>(m_emitterImageName);

		if( resourceHIT == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter '%s' can't compile emitter hit %s"
				, m_name.c_str()
				, m_emitterImageName.c_str()
				);

			return false;
		}

		size_t alphaWidth = resourceHIT->getWidth();
		size_t alphaHeight = resourceHIT->getHeight();

        unsigned char * alphaBuffer = resourceHIT->getBuffer();
        
		if( m_interface->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false)
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
	bool ParticleEmitter::changeEmitterPolygon( const Polygon & _polygon )
	{
        m_polygon = _polygon;

        boost::geometry::correct( m_polygon );

        if( this->isCompile() == false ) 
		{
			return true;
		}

        if( this->compilePolygon_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleEmitter::compilePolygon_()
    {
        size_t n = boost::geometry::num_points( m_polygon );

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
        size_t triangles_size = points.size() / 3;

        if( m_interface->changeEmitterModel( triangles_ptr, triangles_size ) == false)
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEmitter::changeEmitterPolygon '%s' changeEmitterModel Error polygon"
				, m_name.c_str()
				);

			return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter::removeEmitterPolygon()
    {
        m_polygon = Polygon();
        
        if( this->isCompile() == false )
        {
            return;
        }

        m_interface->changeEmitterModel( NULL, 0 );
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_updateBoundingBox( mt::box2f& _boundingBox )
	{
        (void)_boundingBox;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::_invalidateWorldMatrix()
	{
        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterRelative == true )
        {
            const mt::vec3f & wm_pos = this->getWorldPosition();

            m_interface->setPosition( wm_pos );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEmitter::_invalidateColor()
    {
        m_emitterChangeRenderingExtra = true;
    }
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getDuration() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::getDuration '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float duration = m_interface->getDuration();

		return duration;
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getLeftBorder() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::getLeftBorder '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float leftBoard = m_interface->getLeftBorder();

		return leftBoard;
	}
	/////////////////////////////////////////////////////////////////////////
	float ParticleEmitter::getRightBorder() const
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::getRightBorder '%s' can't compile"
                , m_name.c_str()
                );

            return 0.f;
        }

        float rightBoard = m_interface->getRightBorder();

		return rightBoard;
	}
	/////////////////////////////////////////////////////////////////////////
	const ConstString& ParticleEmitter::getEmitterName() const
	{
		return m_emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::box2f ParticleEmitter::getEmitterBoundingBox() const
	{
		mt::box2f box;

        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::getEmitterBoundingBox '%s' can't compile"
                , m_name.c_str()
                );

            return box;
        }

		m_interface->getBoundingBox( box );

		return box;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f ParticleEmitter::getEmitterPosition() const
	{
		mt::vec3f pos;

        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleEmitter::getEmitterPosition '%s' can't compile"
                , m_name.c_str()
                );

            return pos;
        }

		m_interface->getPosition( pos );

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter::setRandomMode( bool _randomMode )
	{
        m_randomMode = _randomMode;

        if( this->isCompile() == false )
        {
            return;
        }

		m_interface->setRandomMode( _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter::getRandomMode() const
	{
        return m_randomMode;
	}
	//////////////////////////////////////////////////////////////////////////
}
