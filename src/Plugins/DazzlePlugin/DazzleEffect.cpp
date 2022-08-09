#include "DazzleEffect.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::DazzleEffect()
        : m_service( nullptr )
        , m_instance( nullptr )
        , m_renderVertices( nullptr )
        , m_renderVertexCount( 0U )
        , m_renderIndicies( nullptr )
        , m_renderIndexCount( 0U )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::~DazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    const dz_service_t * DazzleEffect::getDazzleService() const
    {
        return m_service;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::setResourceDazzle( const ResourcePtr & _resourceDazzleEffect )
    {
        m_resourceDazzleEffect = _resourceDazzleEffect;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & DazzleEffect::getResourceDazzle() const
    {
        return m_resourceDazzleEffect;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::setResourceImage( const ResourceImagePtr & _resource )
    {
        m_resourceImage = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & DazzleEffect::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_play( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_restart( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        if( this->isCompile() == false )
        {
            return true;
        }

        dz_instance_set_time( m_instance, _time * 0.001f );

        dz_instance_emit_resume( m_instance );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_pause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_resume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_stop( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_end( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_interrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        dz_instance_emit_pause( m_instance );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr DazzleEffect::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' resource '%s' m_material is nullptr"
            , this->getName().c_str()
            , m_resourceImage->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    float DazzleEffect::_getDuration() const
    {
        UnknownResourceDazzleEffectInterface * unknownResourceDazzleEffect = m_resourceDazzleEffect->getUnknown();

        DazzleDataInterfacePtr data = unknownResourceDazzleEffect->getData();

        const dz_effect_t * effect = data->getDazzleEffect();

        float duration = dz_effect_get_life( effect ) * 1000.f;

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        dz_instance_set_loop( m_instance, _value == true ? DZ_TRUE : DZ_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_activate()
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_deactivate()
    {
        //ToDo

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceDazzleEffect, "name '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceDazzleEffect->compile() == false )
        {
            LOGGER_ERROR( "dazzle '%s' effect resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceDazzleEffect->getName().c_str()
            );

            return false;
        }

        if( m_resourceImage != nullptr )
        {
            if( m_resourceImage->compile() == false )
            {
                LOGGER_ERROR( "dazzle '%s' image resource '%s' not compile"
                    , this->getName().c_str()
                    , m_resourceImage->getName().c_str()
                );

                return false;
            }
        }

        UnknownResourceDazzleEffectInterface * unknownResourceDazzleEffect = m_resourceDazzleEffect->getUnknown();

        DazzleDataInterfacePtr data = unknownResourceDazzleEffect->getData();

        if( data->acquire() == false )
        {
            LOGGER_ERROR( "dazzle '%s' resource '%s' not acquire composition"
                , this->getName().c_str()
                , m_resourceDazzleEffect->getName().c_str()
            );

            return false;
        }

        const dz_effect_t * effect = data->getDazzleEffect();

        dz_instance_t * instance;
        if( dz_instance_create( m_service, &instance, effect, DZ_NULLPTR ) == DZ_FAILURE )
        {
            LOGGER_ERROR( "dazzle '%s' resource '%s' invalid instance create"
                , this->getName().c_str()
                , m_resourceDazzleEffect->getName().c_str()
            );

            return false;
        }

        m_instance = instance;

        bool loop = this->isLoop();

        dz_instance_set_loop( m_instance, loop == true ? DZ_TRUE : DZ_FALSE );

        float time = this->getTime();

        dz_instance_set_time( m_instance, time * 0.001f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_release()
    {
        dz_instance_destroy( m_service, m_instance );
        m_instance = nullptr;

        UnknownResourceDazzleEffectInterface * unknownResourceDazzleEffect = m_resourceDazzleEffect->getUnknown();

        DazzleDataInterfacePtr data = unknownResourceDazzleEffect->getData();
        data->release();

        m_resourceDazzleEffect->release();

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }

        if( m_renderVertices != nullptr )
        {
            Helper::freeArrayT( m_renderVertices );
            m_renderVertices = nullptr;
            m_renderVertexCount = 0;
        }

        if( m_renderIndicies != nullptr )
        {
            Helper::freeArrayT( m_renderIndicies );
            m_renderIndicies = nullptr;
            m_renderIndexCount = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_dispose()
    {
        Node::_dispose();

        m_resourceDazzleEffect = nullptr;
        m_resourceImage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::update( const UpdateContext * _context )
    {
        if( m_instance == nullptr )
        {
            return;
        }

        if( this->isPlay() == false )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context ) * 0.001f;

        dz_instance_update( m_service, m_instance, totalTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        uint16_t vertexCount;
        uint16_t indexCount;
        dz_instance_compute_bounds( m_instance, &vertexCount, &indexCount );

        if( m_renderVertexCount < vertexCount )
        {
            m_renderVertexCount = vertexCount;

            m_renderVertices = Helper::reallocateArrayT<RenderVertex2D>( m_renderVertices, m_renderVertexCount );

            MENGINE_MEMSET( m_renderVertices, 0x00, m_renderVertexCount * sizeof( RenderVertex2D ) );
        }

        if( m_renderIndexCount < indexCount )
        {
            m_renderIndexCount = indexCount;

            m_renderIndicies = Helper::reallocateArrayT<RenderIndex>( m_renderIndicies, m_renderIndexCount );
        }

        dz_instance_mesh_t mesh;
        mesh.position_buffer = m_renderVertices;
        mesh.position_offset = MENGINE_OFFSETOF( RenderVertex2D, position );
        mesh.position_stride = sizeof( RenderVertex2D );

        mesh.color_buffer = m_renderVertices;
        mesh.color_offset = MENGINE_OFFSETOF( RenderVertex2D, color );
        mesh.color_stride = sizeof( RenderVertex2D );

        mesh.uv_buffer = m_renderVertices;
        mesh.uv_offset = MENGINE_OFFSETOF( RenderVertex2D, uv );
        mesh.uv_stride = sizeof( RenderVertex2D );

        mesh.index_buffer = m_renderIndicies;

        mesh.flags = DZ_EFFECT_MESH_FLAG_NONE;
        mesh.r = 1.f;
        mesh.g = 1.f;
        mesh.b = 1.f;
        mesh.a = 1.f;

        dz_instance_mesh_chunk_t chunks[16];
        uint32_t chunk_count;

        dz_instance_compute_mesh( m_instance, &mesh, chunks, 16, &chunk_count );

        this->updateVertexUV_( m_renderVertices, vertexCount );
        this->updateVertexColor_( m_renderVertices, vertexCount );
        this->updateVertexWM_( m_renderVertices, vertexCount );

        const RenderMaterialInterfacePtr & material = this->getMaterial();

        const mt::box2f * bb = this->getBoundingBox();

        for( uint32_t
            it_chunk = 0,
            it_mesh_end = chunk_count;
            it_chunk != it_mesh_end;
            ++it_chunk )
        {
            const dz_instance_mesh_chunk_t & chunk = chunks[it_chunk];

            _renderPipeline->addRenderObject( _context, material, nullptr, m_renderVertices + chunk.vertex_offset, chunk.vertex_count, m_renderIndicies + chunk.index_offset, chunk.index_count, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        MENGINE_UNUSED( _boundingBox );
        MENGINE_UNUSED( _boundingBoxCurrent );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::updateVertexColor_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const
    {
        Color color;
        this->calcTotalColor( &color );

        if( color.isIdentity() == true )
        {
            return;
        }

        ColorValue_ARGB color_argb = color.getAsARGB();

        for( uint16_t
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
    void DazzleEffect::updateVertexWM_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const
    {
        if( this->isIdentityWorldMatrix() == true )
        {
            return;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint16_t
            it = 0,
            it_end = _verticesCount;
            it != it_end;
            ++it )
        {
            RenderVertex2D & r = _vertices[it];

            mt::vec3f wm_pos;
            mt::mul_v3_v3_m4( wm_pos, r.position, wm );

            r.position = wm_pos;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::updateVertexUV_( RenderVertex2D * const _vertices, uint16_t _verticesCount ) const
    {
        for( uint16_t
            it = 0,
            it_end = _verticesCount;
            it != it_end;
            ++it )
        {
            RenderVertex2D & r = _vertices[it];

            m_resourceImage->correctUVImage( r.uv[0], &(r.uv[0]) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}