#include "ParticleEmitter2.h" 

#include "Kernel/Logger.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

//#include "Kernel/Scene.h"
//#include "Kernel/Layer.h"
#include "Kernel/UnknownResourceImageDataInterface.h"

#include "Interface/ParticleSystemInterface.h"

#include "Interface/ResourceInterface.h"

#include "math/box2.h"
#include "math/angle.h"

#include "Kernel/RenderUtils.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter2::ParticleEmitter2()
		: m_emitter(nullptr)
        , m_randomMode(false)
		, m_vertices(nullptr)
		, m_vertexCount(0)
		, m_indicies(nullptr)
		, m_indexCount(0)
		, m_positionProviderOriginOffset(0.f, 0.f, 0.f)
        , m_emitterPositionRelative(false)
		, m_emitterCameraRelative( false )
        , m_emitterTranslateWithParticle(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitter2::~ParticleEmitter2()
	{
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
			LOGGER_ERROR("ParticleEmitter2::_compile '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceParticle.compile() == false )
        {
            LOGGER_ERROR("ParticleEmitter2::_compile '%s' resource '%s' not compile"
                , m_name.c_str()
                , m_resourceParticle->getName().c_str()
                );

            return false;
        }

		ParticleEmitterInterfacePtr emitter = m_resourceParticle->createEmitter();

		if( emitter == nullptr )
		{
			LOGGER_ERROR("ParticleEmitter2::_compile '%s' can't create emitter source '%s'"
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

		if( m_emitterPositionRelative == true )
		{
			if( emitter->setPositionProvider( this ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::_compile '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return false;
			}
		}
		else
		{
			if( emitter->setPositionProvider( nullptr ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::_compile '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return false;
			}
		}

		if( m_emitterCameraRelative == true )
		{
			if( emitter->setCameraProvider( this ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::_compile '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return false;
			}
		}
		else
		{
			if( emitter->setCameraProvider( nullptr ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::_compile '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return false;
			}
		}

		m_emitter = emitter;

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_release()
	{
        m_emitter = nullptr;

        m_resourceParticle.release();

		Helper::freeMemoryT( m_vertices );
		m_vertices = nullptr;
		m_vertexCount = 0;

		Helper::freeMemoryT( m_indicies );
		m_indicies = nullptr;
		m_indexCount = 0;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_play( uint32_t _enumerator, float _time )
	{
        (void)_enumerator;
        (void)_time;

		if( this->isActivate() == false )
		{
			return false;
		}

		m_emitter->play();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEmitter2::_restart( uint32_t _enumerator, float _time )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

        EVENTABLE_METHOD( this, EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _enumerator, _time );

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
	void ParticleEmitter2::_resume( uint32_t _enumerator, float _time )
	{
		(void)_time;
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

        EVENTABLE_METHOD( this, EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_end( uint32_t _enumerator )
	{
        EVENTABLE_METHOD( this, EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
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
            LOGGER_ERROR("ParticleEmitter::_interrupt '%s' can't compile"
                , m_name.c_str()
                );

			return false;
		}

		m_emitter->interrupt();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_update( float _current, float _time )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		Node::_update( _current, _time );

		bool enabled = APPLICATION_SERVICE()
			->getParticleEnable();

		if( enabled == false )
		{
			return;
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_time -= deltha;
		}

		float speedFactor = this->getAnimationSpeedFactor();
		float time = _time * speedFactor;

		float scretch = this->getStretch();

		float totalTime = time / scretch;

		bool stop;
		m_emitter->update( totalTime, stop );

		if( stop == true )
		{
			this->end();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::_render( const RenderContext * _state )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		bool enabled = APPLICATION_SERVICE()
			->getParticleEnable();

		if( enabled == false )
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

			m_vertices = Helper::reallocateArrayT<RenderVertex2D>( m_vertices, m_vertexCount );
		}

		if( m_indexCount < flush.indexCount )
		{
			m_indexCount = flush.indexCount;

			m_indicies = Helper::reallocateArrayT<RenderIndex>( m_indicies, m_indexCount );
		}

		ParticleMesh meshes[MENGINE_PARTICLE_MAX_MESH];

		if( m_emitter->flushParticles( meshes, MENGINE_PARTICLE_MAX_MESH, m_vertices, m_indicies, flush ) == false )
		{
			return;
		}

		this->updateVertexColor_( m_vertices, flush.vertexCount );

		if( m_emitterPositionRelative == false )
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

			RenderTextureInterfacePtr textures[MENGINE_MAX_TEXTURE_STAGES];

			for( uint32_t i = 0; i != mesh.textures; ++i )
			{
				int textureId = mesh.texture[i];

				const ResourceImagePtr & image = PARTICLE_SYSTEM2()
					->getResourceImage( textureId );

				if( image == nullptr )
				{
					return;
				}

				const RenderTextureInterfacePtr & texture = image->getTexture();

                if( texture == nullptr )
                {
                    return;
                }

				textures[i] = texture;
			}

			const RenderMaterialStage * stage = PARTICLE_SYSTEM2()
				->getMaterialStage( mesh.material );

			RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
				->getMaterial2( STRINGIZE_STRING_LOCAL( "ParticleEmitter2"), stage, PT_TRIANGLELIST, mesh.textures, textures );

            this->addRenderObject( _state, material, m_vertices + mesh.vertexOffset, mesh.vertexCount, m_indicies + mesh.indexOffset, mesh.indexCount, &bb, false );
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
			mt::vec3f & pos = _vertices[it].position;

			mt::vec3f wm_pos;
			mt::mul_v3_v3_m4( wm_pos, pos, wm );

			_vertices[it].position = wm_pos;
		}
    }
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setResourceParticle( const ResourcePtr & _resourceParticle )
	{
		if( m_resourceParticle == _resourceParticle )
		{
			return;
		}

        this->recompile( [this, _resourceParticle]() {m_resourceParticle = stdex::intrusive_static_cast<ResourceParticlePtr>(_resourceParticle); } );
	}
    //////////////////////////////////////////////////////////////////////////
	const ResourcePtr & ParticleEmitter2::getResourceParticle() const
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
	void ParticleEmitter2::setEmitterPositionRelative( bool _positionRelative )
	{
		m_emitterPositionRelative = _positionRelative;

		if( this->isCompile() == false )
		{
			return;
		}

		if( m_emitterPositionRelative == true )
		{
			if( m_emitter->setPositionProvider( this ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::setEmitterPositionRelative '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return;
			}
		}
		else
		{ 
			if( m_emitter->setPositionProvider( nullptr ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::setEmitterPositionRelative '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterCameraRelative( bool _cameraRelative )
	{
		m_emitterCameraRelative = _cameraRelative;

		if( this->isCompile() == false )
		{
			return;
		}

		if( m_emitterCameraRelative == true )
		{
			if( m_emitter->setCameraProvider( this ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::setEmitterCameraRelative '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return;
			}
		}
		else
		{
			if( m_emitter->setCameraProvider( nullptr ) == false )
			{
				LOGGER_ERROR("ParticleEmitter2::setEmitterCameraRelative '%s' group '%s' resource '%s' invalid setup position provider"
					, this->getName().c_str()
					, m_resourceParticle->getGroupName().c_str()
					, m_resourceParticle->getName().c_str()
					);

				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset )
	{
		m_positionProviderOriginOffset = _originOffset;
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
		ResourcePtr resourceHIT = RESOURCE_SERVICE()
			->getResource( m_emitterImageName );

		if( resourceHIT == nullptr )
		{
			LOGGER_ERROR("ParticleEmitter '%s' can't compile emitter hit %s"
                , this->getName().c_str()
				, m_emitterImageName.c_str()
				);

			return false;
		}

        UnknownResourceImageDataInterfacePtr unknownImageData = resourceHIT->getUnknown();

        if( unknownImageData == nullptr )
        {
            LOGGER_ERROR( "ParticleEmitter '%s' resource '%s' for emitter image don't base 'UnknownResourceImageDataInterface'"
                , this->getName().c_str()
                , m_emitterImageName.c_str()
            );

            return false;
        }

		uint32_t alphaWidth = unknownImageData->getImageWidth();
		uint32_t alphaHeight = unknownImageData->getImageHeight();

		uint8_t * alphaBuffer = unknownImageData->getImageBuffer();
        
		if( _emitter->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false )
		{
			LOGGER_ERROR("ParticleEmitter::setEmitterImage %s changeEmitterImage Error image %s"
                , this->getName().c_str()
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

		VectorPoints points;
		if( m_polygon.triangulate( points ) == false )
		{
			LOGGER_ERROR("ParticleEmitter::changeEmitterPolygon '%s' wrong polygon"
				, m_name.c_str()
				);

			return false;
		}

		if( points.empty() == true )
		{
            LOGGER_ERROR("ParticleEmitter::changeEmitterPolygon '%s' empty points"
                , m_name.c_str()
                );

			return false;
		}
        
        float * triangles_ptr = points.front().buff();
        VectorPoints::size_type triangles_size = points.size() / 3;

		if( _emitter->changeEmitterModel( triangles_ptr, (uint32_t)triangles_size ) == false )
		{
			LOGGER_ERROR("ParticleEmitter::changeEmitterPolygon '%s' changeEmitterModel Error polygon"
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
            LOGGER_ERROR("ParticleEmitter::getDuration '%s' can't compile"
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
		mt::vec3f scale; 
		mt::vec2f skew;
		mt::vec3f orientation;
		this->getTransformation( position, origin, scale, skew, orientation );

		mt::mat4f wm;
		this->calcWorldMatrix( wm, position, origin + m_positionProviderOriginOffset, scale, skew, orientation );

		_position = wm.v3.to_vec3f();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEmitter2::onProviderEmitterCamera( bool & _orthogonality, mt::vec3f & _position, mt::vec3f & _direction )
	{
		RenderCameraInterfacePtr camera = this->getRenderCameraInheritance();

		if( camera == nullptr )
		{
			camera = PLAYER_SERVICE()
				->getRenderCamera();
		}

		_orthogonality = true;

		const mt::mat4f & vmi = camera->getCameraViewMatrixInv();

		_position = vmi.v3.to_vec3f();
		_direction = vmi.v0.to_vec3f();
	}
}
