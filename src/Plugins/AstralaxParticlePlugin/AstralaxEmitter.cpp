#include "AstralaxEmitter.h" 

#include "AstralaxInterface.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/UnknownResourceImageDataInterface.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/RenderUtils.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"

#include "math/box2.h"
#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitter::AstralaxEmitter()
        : m_randomMode( false )
        , m_renderVertices( nullptr )
        , m_renderVertexCount( 0 )
        , m_renderIndicies( nullptr )
        , m_renderIndexCount( 0 )
        , m_positionProviderOriginOffset( 0.f, 0.f, 0.f )
        , m_emitterPositionRelative( false )
        , m_emitterCameraRelative( false )
        , m_emitterTranslateWithParticle( true )
        , m_emitterTranslateWithParticleSetup( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitter::~AstralaxEmitter()
    {
    }
    ///////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_deactivate()
    {
        this->stop();

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceParticle, false, "emitter '%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceParticle->compile() == false )
        {
            LOGGER_ERROR( "emitter '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceParticle->getName().c_str()
            );

            return false;
        }

        AstralaxEmitterInterfacePtr emitter = m_resourceParticle->createEmitter( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( emitter, false, "emitter '%s' can't create emitter source '%s'"
            , this->getName().c_str()
            , m_resourceParticle->getName().c_str()
        );

        if( m_emitterTranslateWithParticleSetup == true )
        {
            emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
        }

        if( m_emitterImageName.empty() == false )
        {
            if( this->compileEmitterImage_( emitter ) == false )
            {
                return false;
            }
        }

        emitter->setRandomMode( m_randomMode );

        bool loop = this->isLoop();
        emitter->setLoop( loop );

        Polygon::size_type polygon_count = m_polygon.size();

        if( polygon_count != 0 )
        {
            if( this->compilePolygon_( emitter ) == false )
            {
                return false;
            }
        }

        if( m_emitterPositionRelative == true )
        {
            if( emitter->setPositionProvider( AstralaxPositionProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
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
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceParticle->getGroupName().c_str()
                    , m_resourceParticle->getName().c_str()
                );

                return false;
            }
        }

        if( m_emitterCameraRelative == true )
        {
            if( emitter->setCameraProvider( AstralaxCameraProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
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
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
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
    void AstralaxEmitter::_release()
    {
        m_emitter->setPositionProvider( nullptr );
        m_emitter->setCameraProvider( nullptr );
        m_emitter = nullptr;

        m_resourceParticle->release();

        Helper::freeArrayT( m_renderVertices );
        m_renderVertices = nullptr;
        m_renderVertexCount = 0;

        Helper::freeArrayT( m_renderIndicies );
        m_renderIndicies = nullptr;
        m_renderIndexCount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_play( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        if( this->isActivate() == false )
        {
            return false;
        }

        m_emitter->play();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_restart( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        if( this->isActivate() == false )
        {
            return false;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _enumerator, _time );

        m_emitter->restart();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_pause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( this->isActivate() == false )
        {
            return;
        }

        m_emitter->pause();
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_resume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        if( this->isActivate() == false )
        {
            return;
        }

        m_emitter->resume();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_stop( uint32_t _enumerator )
    {
        if( this->isActivate() == false )
        {
            return false;
        }

        m_emitter->stop();

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->setLoop( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_interrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "emitter '%s' can't compile"
                , this->getName().c_str()
            );

            return false;
        }

        m_emitter->interrupt();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::update( const UpdateContext * _context )
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

        float totalTime = this->calcTotalTime( _context );

        bool stop;
        m_emitter->update( totalTime, &stop );

        if( stop == true )
        {
            this->end();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
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

        AstralaxEmitterRenderFlush flush;
        if( m_emitter->prepareParticles( flush ) == false )
        {
            return;
        }

        if( m_renderVertexCount < flush.vertexCount )
        {
            m_renderVertexCount = flush.vertexCount;

            m_renderVertices = Helper::reallocateArrayT<RenderVertex2D>( m_renderVertices, m_renderVertexCount );
        }

        if( m_renderIndexCount < flush.indexCount )
        {
            m_renderIndexCount = flush.indexCount;

            m_renderIndicies = Helper::reallocateArrayT<RenderIndex>( m_renderIndicies, m_renderIndexCount );
        }

        AstralaxMesh meshes[MENGINE_PARTICLE_MAX_MESH];

        if( m_emitter->flushParticles( meshes, MENGINE_PARTICLE_MAX_MESH, m_renderVertices, m_renderIndicies, flush ) == false )
        {
            return;
        }

        this->updateVertexColor_( m_renderVertices, flush.vertexCount );

        if( m_emitterPositionRelative == false )
        {
            this->updateVertexWM_( m_renderVertices, flush.vertexCount );
        }

        const mt::box2f * bb = this->getBoundingBox();

        for( uint32_t
            it_mesh = 0,
            it_mesh_end = flush.meshCount;
            it_mesh != it_mesh_end;
            ++it_mesh )
        {
            const AstralaxMesh & mesh = meshes[it_mesh];

            RenderTextureInterfacePtr textures[MENGINE_MAX_TEXTURE_STAGES];

            for( uint32_t i = 0; i != mesh.textures; ++i )
            {
                int32_t textureId = mesh.texture[i];

                const ResourceImagePtr & image = ASTRALAX_SYSTEM()
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

            const RenderMaterialStage * stage = ASTRALAX_SYSTEM()
                ->getMaterialStage( mesh.material );

            const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
                ->getMaterial2( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), stage, PT_TRIANGLELIST, mesh.textures, textures, MENGINE_DOCUMENT_FORWARD );

            _renderPipeline->addRenderObject( _context, material, nullptr, m_renderVertices + mesh.vertexOffset, mesh.vertexCount, m_renderIndicies + mesh.indexOffset, mesh.indexCount, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::updateVertexColor_( RenderVertex2D * _vertices, uint32_t _verticesCount ) const
    {
        Color color;
        this->calcTotalColor( color );

        if( color.isIdentity() == true )
        {
            return;
        }

        ColorValue_ARGB color_argb = color.getAsARGB();

        for( uint32_t
            it = 0,
            it_end = _verticesCount;
            it != it_end;
            ++it )
        {
            RenderVertex2D & p = _vertices[it];

            if( p.color == Detail::COLOR_IDENTITY_VALUE )
            {
                p.color = color_argb;
            }
            else
            {
                Color cv( p.color );
                cv *= color;
                p.color = cv.getAsARGB();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::updateVertexWM_( RenderVertex2D * _vertices, uint32_t _verticesCount ) const
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
            const mt::vec3f & pos = _vertices[it].position;

            mt::vec3f wm_pos;
            mt::mul_v3_v3_m4( wm_pos, pos, wm );

            _vertices[it].position = wm_pos;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setResourceParticle( const ResourcePtr & _resourceParticle )
    {
        if( m_resourceParticle == _resourceParticle )
        {
            return;
        }

        this->recompile( [this, &_resourceParticle]()
        {
            m_resourceParticle = stdex::intrusive_static_cast<ResourceAstralaxPtr>(_resourceParticle);

            if( m_resourceParticle == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & AstralaxEmitter::getResourceParticle() const
    {
        return m_resourceParticle;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterTranslateWithParticle( bool _translateWithParticle )
    {
        m_emitterTranslateWithParticle = _translateWithParticle;

        m_emitterTranslateWithParticleSetup = true;

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterPositionRelative( bool _positionRelative )
    {
        m_emitterPositionRelative = _positionRelative;

        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterPositionRelative == true )
        {
            if( m_emitter->setPositionProvider( AstralaxPositionProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
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
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceParticle->getGroupName().c_str()
                    , m_resourceParticle->getName().c_str()
                );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterCameraRelative( bool _cameraRelative )
    {
        m_emitterCameraRelative = _cameraRelative;

        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterCameraRelative == true )
        {
            if( m_emitter->setCameraProvider( AstralaxCameraProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
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
                LOGGER_ERROR( "emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceParticle->getGroupName().c_str()
                    , m_resourceParticle->getName().c_str()
                );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset )
    {
        m_positionProviderOriginOffset = _originOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::changeEmitterImage( const ConstString & _emitterImageName )
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
    void AstralaxEmitter::removeEmitterImage()
    {
        m_emitterImageName.clear();

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->changeEmitterImage( 0, 0, 0, 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::compileEmitterImage_( const AstralaxEmitterInterfacePtr & _emitter )
    {
        const ResourcePtr & resourceHIT = RESOURCE_SERVICE()
            ->getResource( m_emitterImageName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceHIT, false, "emitter '%s' can't compile emitter hit '%s'"
            , this->getName().c_str()
            , m_emitterImageName.c_str()
        );

        UnknownResourceImageDataInterface * unknownImageData = resourceHIT->getUnknown();

        MENGINE_ASSERTION_MEMORY_PANIC( unknownImageData, false, "emitter '%s' resource '%s' for emitter image don't base 'UnknownResourceImageDataInterface'"
            , this->getName().c_str()
            , m_emitterImageName.c_str()
        );

        uint32_t alphaWidth = unknownImageData->getImageWidth();
        uint32_t alphaHeight = unknownImageData->getImageHeight();

        uint8_t * alphaBuffer = unknownImageData->getImageBuffer();

        if( _emitter->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false )
        {
            LOGGER_ERROR( "emitter '%s' changeEmitterImage Error image '%s'"
                , this->getName().c_str()
                , m_emitterImageName.c_str()
            );

            resourceHIT->release();

            return false;
        }

        resourceHIT->release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::changeEmitterPolygon( const Polygon & _polygon )
    {
        m_polygon = _polygon;

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
    bool AstralaxEmitter::compilePolygon_( const AstralaxEmitterInterfacePtr & _emitter )
    {
        Polygon::size_type n = m_polygon.size();

        if( n == 0 )
        {
            return true;
        }

        VectorPoints points;
        if( Helper::triangulate( m_polygon, points ) == false )
        {
            LOGGER_ERROR( "emitter '%s' wrong polygon"
                , this->getName().c_str()
            );

            return false;
        }

        if( points.empty() == true )
        {
            LOGGER_ERROR( "emitter '%s' empty points"
                , this->getName().c_str()
            );

            return false;
        }

        float * triangles_ptr = points.front().buff();
        VectorPoints::size_type triangles_size = points.size() / 3;

        if( _emitter->changeEmitterModel( triangles_ptr, (uint32_t)triangles_size ) == false )
        {
            LOGGER_ERROR( "emitter '%s' changeEmitterModel Error polygon"
                , this->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::removeEmitterPolygon()
    {
        m_polygon = Polygon();

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->changeEmitterModel( nullptr, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const
    {
        const mt::box2f & bb = m_emitter->getBoundingBox();

        _boundingBox = bb;

        *_boundingBoxCurrent = &_boundingBox;
    }
    /////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter::_getDuration() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "emitter '%s' can't compile"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float duration = m_emitter->getDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterRandomMode( bool _randomMode )
    {
        m_randomMode = _randomMode;

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->setRandomMode( _randomMode );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::getEmitterRandomMode() const
    {
        return m_randomMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::onProviderEmitterPosition( mt::vec3f & _position )
    {
        uint8_t transformationFlag;
        mt::vec3f position;
        mt::vec3f origin;
        mt::vec3f scale;
        mt::vec2f skew;
        mt::vec3f orientation;
        this->getTransformation( transformationFlag, position, origin, scale, skew, orientation );

        mt::mat4f wm;
        this->calcWorldMatrix( wm, transformationFlag | TRANSFORMATION_INVALIDATE_ORIGIN, position, origin + m_positionProviderOriginOffset, scale, skew, orientation );

        _position = wm.v3.to_vec3f();
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::onProviderEmitterCamera( bool & _orthogonality, mt::vec3f & _position, mt::vec3f & _direction )
    {
        RenderCameraInterfacePtr camera = Helper::getNodeRenderCameraInheritance( this );

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
