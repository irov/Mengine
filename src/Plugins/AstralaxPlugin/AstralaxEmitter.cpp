#include "AstralaxEmitter.h" 

#include "AstralaxInterface.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/UnknownResourceImageDataInterface.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/MemoryAllocator.h"

#include "math/box2.h"
#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitter::AstralaxEmitter()
        : m_positionProviderOriginOffset( 0.f, 0.f, 0.f )
        , m_emitterPositionRelative( false )
        , m_emitterCameraRelative( false )
        , m_randomMode( false )
        , m_renderVertices( nullptr )
        , m_renderVertexCount( 0U )
        , m_renderIndicies( nullptr )
        , m_renderIndexCount( 0U )
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
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceAstralax, "emitter '%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceAstralax->compile() == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceAstralax->getName().c_str()
            );

            return false;
        }

        AstralaxEmitterInterfacePtr emitter = m_resourceAstralax->createEmitter( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( emitter, "emitter '%s' can't create emitter source '%s'"
            , this->getName().c_str()
            , m_resourceAstralax->getName().c_str()
        );

        if( m_emitterTranslateWithParticleSetup == true )
        {
            emitter->setEmitterTranslateWithParticle( m_emitterTranslateWithParticle );
        }

        if( m_emitterResourceImage != nullptr )
        {
            if( this->compileEmitterImage_( emitter ) == false )
            {
                return false;
            }
        }

        emitter->setRandomMode( m_randomMode );

        bool loop = this->calcTotalLoop();
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
            if( emitter->setPositionProvider( AstralaxPositionProviderInterfacePtr::from( this ) ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return false;
            }
        }
        else
        {
            if( emitter->setPositionProvider( nullptr ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return false;
            }
        }

        if( m_emitterCameraRelative == true )
        {
            if( emitter->setCameraProvider( AstralaxCameraProviderInterfacePtr::from( this ) ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return false;
            }
        }
        else
        {
            if( emitter->setCameraProvider( nullptr ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
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

        m_resourceAstralax->release();

        Helper::freeArrayT( m_renderVertices );
        m_renderVertices = nullptr;
        m_renderVertexCount = 0;

        Helper::freeArrayT( m_renderIndicies );
        m_renderIndicies = nullptr;
        m_renderIndexCount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::_play( UniqueId _enumerator, float _time )
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
    bool AstralaxEmitter::_restart( UniqueId _enumerator, float _time )
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
    void AstralaxEmitter::_pause( UniqueId _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( this->isActivate() == false )
        {
            return;
        }

        m_emitter->pause();
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::_resume( UniqueId _enumerator, float _time )
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
    bool AstralaxEmitter::_stop( UniqueId _enumerator )
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
    void AstralaxEmitter::_end( UniqueId _enumerator )
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
    bool AstralaxEmitter::_interrupt( UniqueId _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' can't compile"
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
        if( m_emitter->prepareParticles( &flush ) == false )
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
        if( m_emitter->flushParticles( meshes, MENGINE_PARTICLE_MAX_MESH, m_renderVertices, m_renderIndicies, &flush ) == false )
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

            const RenderMaterialInterfacePtr & material = ASTRALAX_SERVICE()
                ->getMaterial( mesh );

            if( material == nullptr )
            {
                return;
            }

            _renderPipeline->addRenderObject( _context, material, nullptr, m_renderVertices + mesh.vertexOffset, mesh.vertexCount, m_renderIndicies + mesh.indexOffset, mesh.indexCount, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::updateVertexColor_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const
    {
        Color color;
        this->calcTotalColor( &color );

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
                Color cv = Helper::makeColorARGB( p.color );
                cv *= color;
                p.color = cv.getAsARGB();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::updateVertexWM_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const
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
            RenderVertex2D & r = _vertices[it];

            mt::vec3f wm_pos;
            mt::mul_v3_v3_m4( &wm_pos, r.position, wm );

            r.position = wm_pos;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setResourceAstralax( const ResourcePtr & _resourceParticle )
    {
        MENGINE_ASSERTION_RESOURCE_TYPE( _resourceParticle, ResourceAstralaxPtr, "Resource '%s' invalid type '%s' is not 'ResourceAstralax'"
            , _resourceParticle == nullptr ? "nullptr" : _resourceParticle->getName().c_str()
            , _resourceParticle == nullptr ? "nullptr" : _resourceParticle->getType().c_str()
        );

        if( m_resourceAstralax == _resourceParticle )
        {
            return;
        }

        this->recompile( [this, &_resourceParticle]()
        {
            m_resourceAstralax = ResourceAstralaxPtr::from( _resourceParticle );

            if( m_resourceAstralax == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & AstralaxEmitter::getResourceAstralax() const
    {
        return m_resourceAstralax;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterTranslateWithParticle( bool _translateWithParticle )
    {
        if( m_emitterTranslateWithParticle == _translateWithParticle )
        {
            return;
        }

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
        if( m_emitterPositionRelative == _positionRelative )
        {
            return;
        }

        m_emitterPositionRelative = _positionRelative;

        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterPositionRelative == true )
        {
            if( m_emitter->setPositionProvider( AstralaxPositionProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return;
            }
        }
        else
        {
            if( m_emitter->setPositionProvider( nullptr ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::setEmitterCameraRelative( bool _cameraRelative )
    {
        if( m_emitterCameraRelative == _cameraRelative )
        {
            return;
        }

        m_emitterCameraRelative = _cameraRelative;

        if( this->isCompile() == false )
        {
            return;
        }

        if( m_emitterCameraRelative == true )
        {
            if( m_emitter->setCameraProvider( AstralaxCameraProviderInterfacePtr( this ) ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
                );

                return;
            }
        }
        else
        {
            if( m_emitter->setCameraProvider( nullptr ) == false )
            {
                LOGGER_ERROR( "astralax emitter '%s' group '%s' resource '%s' invalid setup position provider"
                    , this->getName().c_str()
                    , m_resourceAstralax->getGroupName().c_str()
                    , m_resourceAstralax->getName().c_str()
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
    void AstralaxEmitter::changeEmitterImage( const ResourceImagePtr & _emitterResourceImage )
    {
        if( m_emitterResourceImage == _emitterResourceImage )
        {
            return;
        }

        m_emitterResourceImage = _emitterResourceImage;

        if( this->isCompile() == false )
        {
            return;
        }

        this->compileEmitterImage_( m_emitter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::removeEmitterImage()
    {
        m_emitterResourceImage = nullptr;

        if( this->isCompile() == false )
        {
            return;
        }

        m_emitter->changeEmitterImage( 0, 0, nullptr, 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::compileEmitterImage_( const AstralaxEmitterInterfacePtr & _emitter )
    {
        if( m_emitterResourceImage->compile() == false )
        {
            return false;
        }

        UnknownResourceImageDataInterface * unknownImageData = m_emitterResourceImage->getUnknown();

        MENGINE_ASSERTION_MEMORY_PANIC( unknownImageData, "emitter '%s' resource '%s' for emitter image don't base 'UnknownResourceImageDataInterface'"
            , this->getName().c_str()
            , m_emitterResourceImage->getName().c_str()
        );

        uint32_t alphaWidth = unknownImageData->getImageWidth();
        uint32_t alphaHeight = unknownImageData->getImageHeight();

        uint8_t * alphaBuffer = unknownImageData->getImageBuffer();

        if( _emitter->changeEmitterImage( alphaWidth, alphaHeight, alphaBuffer, 1 ) == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' changeEmitterImage Error image '%s'"
                , this->getName().c_str()
                , m_emitterResourceImage->getName().c_str()
            );

            m_emitterResourceImage->release();

            return false;
        }

        m_emitterResourceImage->release();

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
        if( Helper::triangulate( m_polygon, &points ) == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' wrong polygon"
                , this->getName().c_str()
            );

            return false;
        }

        if( points.empty() == true )
        {
            LOGGER_ERROR( "astralax emitter '%s' empty points"
                , this->getName().c_str()
            );

            return false;
        }

        float * triangles_ptr = points.front().buff();
        VectorPoints::size_type triangles_size = points.size() / 3;

        if( _emitter->changeEmitterModel( triangles_ptr, (uint32_t)triangles_size ) == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' changeEmitterModel Error polygon"
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
    void AstralaxEmitter::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const mt::box2f & bb = m_emitter->getBoundingBox();

        *_boundingBox = bb;

        *_boundingBoxCurrent = _boundingBox;
    }
    /////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter::_getDuration() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "astralax emitter '%s' can't compile"
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
        if( m_randomMode == _randomMode )
        {
            return;
        }

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
    void AstralaxEmitter::onProviderEmitterPosition( mt::vec3f * _position )
    {
        uint8_t transformationFlag;
        mt::vec3f position;
        mt::vec3f origin;
        mt::vec3f scale;
        mt::vec2f skew;
        mt::vec3f orientation;
        this->getTransformationData( &transformationFlag, &position, &origin, &scale, &skew, &orientation );

        mt::mat4f wm;
        this->calcWorldMatrix( &wm, transformationFlag | TRANSFORMATION_INVALIDATE_ORIGIN, position, origin + m_positionProviderOriginOffset, scale, skew, orientation );

        *_position = wm.v3.to_vec3f();
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::onProviderEmitterCamera( bool * const _orthogonality, mt::vec3f * const _position, mt::vec3f * const _direction )
    {
        RenderCameraInterfacePtr camera = Helper::getNodeRenderCameraInheritance( this );

        if( camera == nullptr )
        {
            camera = PLAYER_SERVICE()
                ->getRenderCamera();
        }

        *_orthogonality = true;

        const mt::mat4f & vmi = camera->getCameraViewMatrixInv();

        *_position = vmi.v3.to_vec3f();
        *_direction = vmi.v0.to_vec3f();
    }
    //////////////////////////////////////////////////////////////////////////
}
