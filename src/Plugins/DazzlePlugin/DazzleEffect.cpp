#include "DazzleEffect.h"

#include "Interface/RenderBatchInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Materialable.h"
#include "Kernel/MemoryAllocator.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleEffect::DazzleEffect()
        : m_service( nullptr )
        , m_instance( nullptr )
        , m_renderVertices( nullptr )
        , m_renderNormals( nullptr )
        , m_renderTangents( nullptr )
        , m_renderColors( nullptr )
        , m_renderVertexCount( MENGINE_UINT32_C( 0 ) )
        , m_dazzleIndices( nullptr )
        , m_dazzleIndexSize( 0 )
        , m_renderChunks( nullptr )
        , m_renderChunkCount( MENGINE_UINT32_C( 0 ) )
        , m_submitVertices( nullptr )
        , m_renderIndicies( nullptr )
        , m_renderIndexCount( MENGINE_UINT32_C( 0 ) )
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
    bool DazzleEffect::setPathPoints( const VectorDazzlePathPositions & _points )
    {
        bool result = this->setPathPoints_( _points, 0.30f, 1.f, 0.08f, 1.f );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::setPathPointsColored( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors )
    {
        bool result = this->setPathPointsColored_( _points, _colors, 0.30f, 1.f, 0.08f, 1.f );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::setPathPointsColoredGradient( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd )
    {
        bool result = this->setPathPointsColored_( _points, _colors, _scaleBegin, _scaleEnd, _alphaBegin, _alphaEnd );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::setPathPoints_( const VectorDazzlePathPositions & _points, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd )
    {
        VectorDazzlePathPoints pathPoints;
        if( this->makePathPoints_( _points, _scaleBegin, _scaleEnd, _alphaBegin, _alphaEnd, &pathPoints ) == false )
        {
            return false;
        }

        bool result = this->applyPathPoints_( pathPoints );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::setPathPointsColored_( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd )
    {
        VectorDazzlePathPoints pathPoints;
        if( this->makePathPoints_( _points, _scaleBegin, _scaleEnd, _alphaBegin, _alphaEnd, &pathPoints ) == false )
        {
            return false;
        }

        if( this->applyPathPointColors_( _points, _colors, &pathPoints ) == false )
        {
            return false;
        }

        bool result = this->applyPathPoints_( pathPoints );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::makePathPoints_( const VectorDazzlePathPositions & _points, float _scaleBegin, float _scaleEnd, float _alphaBegin, float _alphaEnd, VectorDazzlePathPoints * const _pathPoints ) const
    {
        const VectorDazzlePathPositions::size_type pointsSize = _points.size();

        if( pointsSize < 2 )
        {
            return false;
        }

        _pathPoints->reserve( pointsSize );

        uint32_t segmentPoint = 0U;
        uint32_t segmentCount = 0U;

        for( VectorDazzlePathPositions::size_type index = 0; index != pointsSize; ++index )
        {
            const mt::vec2f & point = _points[index];

            if( point.x <= -99990.f && point.y <= -99990.f )
            {
                segmentPoint = 0U;
                segmentCount = 0U;

                continue;
            }

            if( segmentCount == 0U )
            {
                for( VectorDazzlePathPositions::size_type countIndex = index; countIndex != pointsSize; ++countIndex )
                {
                    const mt::vec2f & countPoint = _points[countIndex];

                    if( countPoint.x <= -99990.f && countPoint.y <= -99990.f )
                    {
                        break;
                    }

                    ++segmentCount;
                }
            }

            const float progress = segmentCount > 1U ? (float)segmentPoint / (float)(segmentCount - 1U) : 1.f;

            dz_path_point_t pathPoint;
            pathPoint.position = {point.x, point.y, 0.f};
            pathPoint.scale = _scaleBegin + (_scaleEnd - _scaleBegin) * progress;
            pathPoint.alpha = _alphaBegin + (_alphaEnd - _alphaBegin) * progress;
            pathPoint.color = {1.f, 1.f, 1.f, 1.f};
            pathPoint.flags = DZ_PATH_POINT_FLAG_NONE;

            if( segmentPoint == 0U )
            {
                pathPoint.flags |= DZ_PATH_POINT_FLAG_BREAK;
            }

            _pathPoints->emplace_back( pathPoint );

            ++segmentPoint;
        }

        const VectorDazzlePathPoints::size_type pathPointsSize = _pathPoints->size();

        if( pathPointsSize < 2 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::applyPathPointColors_( const VectorDazzlePathPositions & _points, const VectorDazzlePathColors & _colors, VectorDazzlePathPoints * const _pathPoints ) const
    {
        const VectorDazzlePathPositions::size_type pointsSize = _points.size();
        const VectorDazzlePathColors::size_type colorsSize = _colors.size();

        if( pointsSize != colorsSize )
        {
            return false;
        }

        const VectorDazzlePathPoints::size_type pathPointsSize = _pathPoints->size();

        VectorDazzlePathPoints::size_type pathIndex = 0;

        for( VectorDazzlePathPositions::size_type index = 0; index != pointsSize; ++index )
        {
            const mt::vec2f & point = _points[index];

            if( point.x <= -99990.f && point.y <= -99990.f )
            {
                continue;
            }

            if( pathIndex + 1 != pathPointsSize && ((*_pathPoints)[pathIndex + 1].flags & DZ_PATH_POINT_FLAG_BREAK) == 0U )
            {
                const mt::vec4f & color = _colors[index];
                dz_path_point_t & pathPoint = (*_pathPoints)[pathIndex];

                pathPoint.color = {color.x, color.y, color.z, color.w};
                pathPoint.flags |= DZ_PATH_POINT_FLAG_COLOR;
            }

            ++pathIndex;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::applyPathPoints_( const VectorDazzlePathPoints & _pathPoints )
    {
        if( m_instance == nullptr )
        {
            return false;
        }

        const dz_path_point_t * pathPoints = _pathPoints.data();
        const VectorDazzlePathPoints::size_type pathPointsSize = _pathPoints.size();

        dz_result_t result = dz_instance_set_path_points( m_service, m_instance, pathPoints, (dz_uint32_t)pathPointsSize );

        if( result == DZ_SUCCESSFUL )
        {
            this->invalidateBoundingBox();
        }

        return result == DZ_SUCCESSFUL;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::clearPathPoints()
    {
        if( m_instance == nullptr )
        {
            return;
        }

        dz_instance_clear_path_points( m_instance );
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::primePath( float _time )
    {
        if( m_instance == nullptr )
        {
            return false;
        }

        dz_instance_restart( m_instance );

        dz_result_t result = dz_instance_seek( m_service, m_instance, _time, 1.f / 120.f );

        if( result != DZ_SUCCESSFUL )
        {
            return false;
        }

        dz_instance_pause( m_instance );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::restartEffect( float _prewarm )
    {
        UniqueId playId = BaseAnimation::play( 0.f );

        if( playId == INVALID_UNIQUE_ID )
        {
            return false;
        }

        if( m_instance == nullptr )
        {
            return true;
        }

        if( _prewarm > 0.f )
        {
            dz_result_t result = dz_instance_update( m_service, m_instance, _prewarm );

            if( result != DZ_SUCCESSFUL )
            {
                return false;
            }
        }

        this->invalidateBoundingBox();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_play( UniqueId _enumerator, float _time )
    {
        return this->_restart( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_restart( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        if( this->isCompile() == false )
        {
            return true;
        }

        dz_instance_restart( m_instance );

        float time = _time * 0.001f;

        if( time > 0.f )
        {
            dz_result_t result = dz_instance_seek( m_service, m_instance, time, dz_instance_get_fixed_step( m_instance ) );
            if( result != DZ_SUCCESSFUL )
            {
                LOGGER_ERROR( "dazzle '%s' seek to '%f' failed result '%u'", this->getName().c_str(), time, (uint32_t)result );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_pause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( m_instance != nullptr )
        {
            dz_instance_pause( m_instance );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_resume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        if( m_instance != nullptr )
        {
            dz_instance_resume( m_instance );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_stop( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( m_instance != nullptr )
        {
            dz_instance_stop( m_instance );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_end( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( m_instance != nullptr )
        {
            dz_instance_stop( m_instance );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::_interrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        if( m_instance != nullptr )
        {
            dz_instance_emit_pause( m_instance );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr DazzleEffect::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material;

        if( m_resourceImage != nullptr )
        {
            material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            material = this->makeSolidMaterial( false, MENGINE_DOCUMENT_FACTORABLE );
        }

        MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' dazzle material is nullptr", this->getName().c_str() );

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

                m_resourceDazzleEffect->release();

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

            m_resourceDazzleEffect->release();

            if( m_resourceImage != nullptr )
            {
                m_resourceImage->release();
            }

            return false;
        }

        const dz_effect_t * effect = data->getDazzleEffect();

        dz_instance_t * instance;
        dz_instance_create( m_service, &instance, effect, DZ_NULLPTR );

        m_instance = instance;

        bool loop = this->isLoop();

        dz_instance_set_loop( m_instance, loop == true ? DZ_TRUE : DZ_FALSE );

        float time = this->getTime();

        if( time > 0.f )
        {
            dz_result_t seekResult = dz_instance_seek( m_service, m_instance, time * 0.001f, dz_instance_get_fixed_step( m_instance ) );
            if( seekResult != DZ_SUCCESSFUL )
            {
                LOGGER_ERROR( "dazzle '%s' resource '%s' invalid initial seek result '%u'", this->getName().c_str(), m_resourceDazzleEffect->getName().c_str(),
                              (uint32_t)seekResult );

                dz_instance_destroy( m_service, m_instance );
                m_instance = nullptr;

                data->release();
                m_resourceDazzleEffect->release();

                if( m_resourceImage != nullptr )
                {
                    m_resourceImage->release();
                }

                return false;
            }
        }

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
        }

        if( m_renderNormals != nullptr )
        {
            Helper::freeArrayT( m_renderNormals );
            m_renderNormals = nullptr;
        }

        if( m_renderTangents != nullptr )
        {
            Helper::freeArrayT( m_renderTangents );
            m_renderTangents = nullptr;
        }

        if( m_renderColors != nullptr )
        {
            Helper::freeArrayT( m_renderColors );
            m_renderColors = nullptr;
        }

        m_renderVertexCount = 0;

        if( m_dazzleIndices != nullptr )
        {
            Helper::freeArrayT( m_dazzleIndices );
            m_dazzleIndices = nullptr;
            m_dazzleIndexSize = 0;
        }

        if( m_renderChunks != nullptr )
        {
            Helper::freeArrayT( m_renderChunks );
            m_renderChunks = nullptr;
            m_renderChunkCount = 0;
        }

        if( m_submitVertices != nullptr )
        {
            Helper::freeArrayT( m_submitVertices );
            m_submitVertices = nullptr;
        }

        if( m_renderIndicies != nullptr )
        {
            Helper::freeArrayT( m_renderIndicies );
            m_renderIndicies = nullptr;
            m_renderIndexCount = 0;
        }

        m_renderMaterials.clear();
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

        float totalTime = this->calcTotalTime( _context );

        float totalTimeMilliseconds = totalTime * 0.001f;

        dz_result_t result = dz_instance_update( m_service, m_instance, totalTimeMilliseconds );

        if( result != DZ_SUCCESSFUL )
        {
            LOGGER_ERROR( "dazzle '%s' update failed result '%u'", this->getName().c_str(), (uint32_t)result );

            return;
        }

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_instance == nullptr )
        {
            return;
        }

        dz_camera_state_t camera;
        const dz_camera_state_t * cameraOverride = this->makeCameraState_( _context, &camera ) == true ? &camera : DZ_NULLPTR;

        dz_render_requirements_t requirements;
        dz_instance_prepare_render( m_instance, cameraOverride, &requirements );

        if( requirements.vertex_count == 0 || requirements.index_count == 0 || requirements.chunk_count == 0 )
        {
            return;
        }

        if( m_renderVertexCount < requirements.vertex_count )
        {
            m_renderVertexCount = requirements.vertex_count;

            m_renderVertices = Helper::reallocateArrayT<RenderVertex2D>( m_renderVertices, m_renderVertexCount );
            m_renderNormals = Helper::reallocateArrayT<dz_vec3_t>( m_renderNormals, m_renderVertexCount );
            m_renderTangents = Helper::reallocateArrayT<dz_vec4_t>( m_renderTangents, m_renderVertexCount );
            m_renderColors = Helper::reallocateArrayT<dz_vec4_t>( m_renderColors, m_renderVertexCount );
        }

        size_t dazzleIndexElementSize = requirements.index_type == DZ_INDEX_UINT32 ? sizeof( dz_uint32_t ) : sizeof( dz_uint16_t );
        size_t dazzleIndexSize = (size_t)requirements.index_count * dazzleIndexElementSize;

        if( m_dazzleIndexSize < dazzleIndexSize )
        {
            m_dazzleIndexSize = dazzleIndexSize;
            m_dazzleIndices = Helper::reallocateArrayT<uint8_t>( m_dazzleIndices, m_dazzleIndexSize );
        }

        if( m_renderChunkCount < requirements.chunk_count )
        {
            m_renderChunkCount = requirements.chunk_count;
            m_renderChunks = Helper::reallocateArrayT<dz_render_chunk_t>( m_renderChunks, m_renderChunkCount );
        }

        dz_render_buffers_t buffers;
        buffers.positions.buffer = m_renderVertices;
        buffers.positions.size = (size_t)m_renderVertexCount * sizeof( RenderVertex2D );
        buffers.positions.offset = MENGINE_OFFSETOF( RenderVertex2D, position );
        buffers.positions.stride = sizeof( RenderVertex2D );

        buffers.normals.buffer = m_renderNormals;
        buffers.normals.size = (size_t)m_renderVertexCount * sizeof( dz_vec3_t );
        buffers.normals.offset = 0;
        buffers.normals.stride = sizeof( dz_vec3_t );

        buffers.tangents.buffer = m_renderTangents;
        buffers.tangents.size = (size_t)m_renderVertexCount * sizeof( dz_vec4_t );
        buffers.tangents.offset = 0;
        buffers.tangents.stride = sizeof( dz_vec4_t );

        buffers.colors.buffer = m_renderColors;
        buffers.colors.size = (size_t)m_renderVertexCount * sizeof( dz_vec4_t );
        buffers.colors.offset = 0;
        buffers.colors.stride = sizeof( dz_vec4_t );

        buffers.uv0.buffer = m_renderVertices;
        buffers.uv0.size = (size_t)m_renderVertexCount * sizeof( RenderVertex2D );
        buffers.uv0.offset = MENGINE_OFFSETOF( RenderVertex2D, uv );
        buffers.uv0.stride = sizeof( RenderVertex2D );

        buffers.uv1.buffer = m_renderVertices;
        buffers.uv1.size = (size_t)m_renderVertexCount * sizeof( RenderVertex2D );
        buffers.uv1.offset = MENGINE_OFFSETOF( RenderVertex2D, uv ) + sizeof( mt::vec2f );
        buffers.uv1.stride = sizeof( RenderVertex2D );

        buffers.indices = m_dazzleIndices;
        buffers.indices_size = m_dazzleIndexSize;
        buffers.index_type = requirements.index_type;

        uint32_t chunkCount = 0;
        dz_result_t fillResult = dz_instance_fill_render( m_instance, cameraOverride, &buffers, m_renderChunks, m_renderChunkCount, &chunkCount );
        if( fillResult != DZ_SUCCESSFUL )
        {
            LOGGER_ERROR( "dazzle '%s' fill render failed result '%u'", this->getName().c_str(), (uint32_t)fillResult );

            return;
        }

        size_t submitIndexCount = 0;

        for( uint32_t index = 0; index != chunkCount; ++index )
        {
            const dz_render_chunk_t & chunk = m_renderChunks[index];

            if( chunk.index_offset > requirements.index_count || chunk.index_count > requirements.index_count - chunk.index_offset ||
                chunk.vertex_offset > requirements.vertex_count || chunk.vertex_count > requirements.vertex_count - chunk.vertex_offset )
            {
                LOGGER_ERROR( "dazzle '%s' invalid render chunk '%u' range", this->getName().c_str(), index );

                return;
            }

            if( chunk.index_count > (size_t)MENGINE_UINT32_C( 0xFFFFFFFF ) - submitIndexCount )
            {
                LOGGER_ERROR( "dazzle '%s' render submission exceeds uint32 limit", this->getName().c_str() );

                return;
            }

            submitIndexCount += chunk.index_count;
        }

        if( m_renderIndexCount < (uint32_t)submitIndexCount )
        {
            m_renderIndexCount = (uint32_t)submitIndexCount;
            m_submitVertices = Helper::reallocateArrayT<RenderVertex2D>( m_submitVertices, m_renderIndexCount );
            m_renderIndicies = Helper::reallocateArrayT<RenderIndex>( m_renderIndicies, m_renderIndexCount );
        }

        this->updateVertexUV_( m_renderVertices, requirements.vertex_count );
        this->updateVertexColor_( m_renderVertices, requirements.vertex_count );
        this->updateVertexWM_( m_renderVertices, requirements.vertex_count );

        const mt::box2f * bb = this->getBoundingBox();
        uint32_t submitOffset = 0;

        for( uint32_t index = 0; index != chunkCount; ++index )
        {
            const dz_render_chunk_t & chunk = m_renderChunks[index];

            uint32_t primitiveIndexCount = chunk.primitive == DZ_PRIMITIVE_LINES ? 2U : 3U;
            if( chunk.primitive != DZ_PRIMITIVE_TRIANGLES && chunk.primitive != DZ_PRIMITIVE_LINES )
            {
                LOGGER_ERROR( "dazzle '%s' render chunk '%u' has unsupported primitive '%u'", this->getName().c_str(), index, (uint32_t)chunk.primitive );

                return;
            }

            if( chunk.index_count % primitiveIndexCount != 0 )
            {
                LOGGER_ERROR( "dazzle '%s' render chunk '%u' has invalid index count '%u'", this->getName().c_str(), index, chunk.index_count );

                return;
            }

            RenderMaterialInterfacePtr material = this->makeMaterialPass_( chunk.pass, chunk.primitive );
            if( material == nullptr )
            {
                LOGGER_ERROR( "dazzle '%s' render chunk '%u' technique '%s' has no material", this->getName().c_str(), index, chunk.pass.technique_id );

                return;
            }

            uint32_t chunkIndexOffset = 0;

            while( chunkIndexOffset != chunk.index_count )
            {
                uint32_t batchIndexCount = chunk.index_count - chunkIndexOffset;

                if( batchIndexCount >= MENGINE_RENDER_VERTEX_MAX_BATCH )
                {
                    batchIndexCount = MENGINE_RENDER_VERTEX_MAX_BATCH - 1U;
                    batchIndexCount -= batchIndexCount % primitiveIndexCount;
                }

                for( uint32_t batchIndex = 0; batchIndex != batchIndexCount; ++batchIndex )
                {
                    uint32_t sourceIndex = this->getRenderIndex_( requirements.index_type, chunk.index_offset + chunkIndexOffset + batchIndex );
                    if( sourceIndex >= requirements.vertex_count )
                    {
                        LOGGER_ERROR( "dazzle '%s' render chunk '%u' has invalid vertex index '%u'", this->getName().c_str(), index, sourceIndex );

                        return;
                    }

                    m_submitVertices[submitOffset + batchIndex] = m_renderVertices[sourceIndex];
                    m_renderIndicies[submitOffset + batchIndex] = (RenderIndex)batchIndex;
                }

                _renderPipeline->addRenderObject( _context, material, nullptr, m_submitVertices + submitOffset, batchIndexCount, m_renderIndicies + submitOffset, batchIndexCount,
                                                  bb, false, MENGINE_DOCUMENT_FORWARD );

                submitOffset += batchIndexCount;
                chunkIndexOffset += batchIndexCount;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        if( m_instance == nullptr )
        {
            *_boundingBoxCurrent = nullptr;

            return;
        }

        dz_aabb_t aabb;
        dz_instance_get_aabb( m_instance, &aabb );
        if( aabb.valid == DZ_FALSE )
        {
            *_boundingBoxCurrent = nullptr;

            return;
        }

        const mt::mat4f & wm = this->getWorldMatrix();
        bool initialized = false;

        for( uint32_t index = 0; index != 8; ++index )
        {
            mt::vec3f localPoint( ( index & 1U ) != 0 ? aabb.maximum.x : aabb.minimum.x, ( index & 2U ) != 0 ? aabb.maximum.y : aabb.minimum.y,
                                  -( ( index & 4U ) != 0 ? aabb.maximum.z : aabb.minimum.z ) );

            mt::vec3f worldPoint;
            mt::mul_v3_v3_m4( &worldPoint, localPoint, wm );

            if( initialized == false )
            {
                mt::box2_reset( _boundingBox, worldPoint.x, worldPoint.y );
                initialized = true;
            }
            else
            {
                mt::box2_add_internal_point( _boundingBox, worldPoint.x, worldPoint.y );
            }
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffect::makeCameraState_( const RenderContext * _context, dz_camera_state_t * const _camera ) const
    {
        const dz_effect_t * effect = dz_instance_get_effect( m_instance );

        dz_project_profile_t profile;
        dz_effect_get_project_profile( effect, &profile );

        float viewportWidth = 1.f;
        float viewportHeight = 1.f;

        if( _context != nullptr && _context->viewport != nullptr )
        {
            const Viewport & viewport = _context->viewport->getViewportWM();
            viewportWidth = viewport.getWidth();
            viewportHeight = viewport.getHeight();
        }
        else if( _context != nullptr && _context->resolution != nullptr )
        {
            const Resolution & resolution = _context->resolution->getContentResolution();
            viewportWidth = resolution.getWidthF();
            viewportHeight = resolution.getHeightF();
        }

        dz_camera_state_from_profile( &profile, viewportWidth, viewportHeight, _camera );

        if( _context == nullptr || _context->camera == nullptr )
        {
            return true;
        }

        const mt::mat4f & cameraWorldMatrix = _context->camera->getCameraViewMatrixInv();

        mt::vec3f worldPosition( cameraWorldMatrix.v3.x, cameraWorldMatrix.v3.y, cameraWorldMatrix.v3.z );
        mt::vec3f worldForward( cameraWorldMatrix.v2.x, cameraWorldMatrix.v2.y, cameraWorldMatrix.v2.z );
        mt::vec3f worldUp( cameraWorldMatrix.v1.x, cameraWorldMatrix.v1.y, cameraWorldMatrix.v1.z );

        mt::mat4f worldMatrixInv;
        mt::inv_m4_m4( &worldMatrixInv, this->getWorldMatrix() );

        mt::vec3f localPosition;
        mt::mul_v3_v3_m4( &localPosition, worldPosition, worldMatrixInv );

        mt::vec3f localForward;
        mt::mul_v3_v3_m4_r( &localForward, worldForward, worldMatrixInv );
        mt::norm_v3_v3( &localForward, localForward );

        mt::vec3f localUp;
        mt::mul_v3_v3_m4_r( &localUp, worldUp, worldMatrixInv );
        mt::norm_v3_v3( &localUp, localUp );

        _camera->position.x = localPosition.x;
        _camera->position.y = localPosition.y;
        _camera->position.z = -localPosition.z;
        _camera->forward.x = localForward.x;
        _camera->forward.y = localForward.y;
        _camera->forward.z = -localForward.z;
        _camera->up.x = localUp.x;
        _camera->up.y = localUp.y;
        _camera->up.z = -localUp.z;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr DazzleEffect::makeMaterialPass_( const dz_material_pass_desc_t & _pass, dz_primitive_type_e _primitive ) const
    {
        if( _primitive == DZ_PRIMITIVE_LINES )
        {
            return RENDERMATERIAL_SERVICE()->getDebugLineMaterial();
        }

        EMaterialBlendMode blendMode = EMB_NORMAL;

        switch( _pass.blend )
        {
        case DZ_BLEND_ADD:
            blendMode = EMB_ADD;
            break;
        case DZ_BLEND_MULTIPLY:
            blendMode = EMB_MULTIPLY;
            break;
        case DZ_BLEND_SCREEN:
            blendMode = EMB_SCREEN;
            break;
        case DZ_BLEND_NORMAL:
        default:
            blendMode = EMB_NORMAL;
            break;
        }

        bool colorTechnique = StdString::strcmp( _pass.technique_id, "dazzle.color" ) == 0;
        bool standardTechnique =
            colorTechnique == true || StdString::strcmp( _pass.technique_id, "dazzle.textured" ) == 0 || StdString::strcmp( _pass.technique_id, "dazzle.lit" ) == 0;

        ConstString materialName = standardTechnique == true ? ConstString::none() : Helper::stringizeString( _pass.technique_id );

        RenderMaterialInterfacePtr material;

        if( colorTechnique == false && m_resourceImage != nullptr )
        {
            material = Helper::makeImageMaterial( m_resourceImage, materialName, blendMode, false, false, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            material = Helper::makeSolidMaterial( materialName, blendMode, false, MENGINE_DOCUMENT_FACTORABLE );
        }

        if( material == nullptr && standardTechnique == false )
        {
            LOGGER_WARNING( "dazzle '%s' technique '%s' is not registered, use standard fallback", this->getName().c_str(), _pass.technique_id );

            if( m_resourceImage != nullptr )
            {
                material = Helper::makeImageMaterial( m_resourceImage, ConstString::none(), blendMode, false, false, MENGINE_DOCUMENT_FACTORABLE );
            }
            else
            {
                material = Helper::makeSolidMaterial( ConstString::none(), blendMode, false, MENGINE_DOCUMENT_FACTORABLE );
            }
        }

        if( material != nullptr )
        {
            bool cached = false;

            for( const RenderMaterialInterfacePtr & cachedMaterial : m_renderMaterials )
            {
                if( cachedMaterial == material )
                {
                    cached = true;
                    break;
                }
            }

            if( cached == false )
            {
                m_renderMaterials.emplace_back( material );
            }
        }

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DazzleEffect::getRenderIndex_( dz_index_type_e _indexType, uint32_t _index ) const
    {
        if( _indexType == DZ_INDEX_UINT32 )
        {
            const dz_uint32_t * indices = (const dz_uint32_t *)m_dazzleIndices;

            return indices[_index];
        }

        const dz_uint16_t * indices = (const dz_uint16_t *)m_dazzleIndices;

        return indices[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::updateVertexColor_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const
    {
        Color color;
        this->calcTotalColor( &color );

        for( uint32_t it = 0, it_end = _verticesCount; it != it_end; ++it )
        {
            RenderVertex2D & p = _vertices[it];
            const dz_vec4_t & particleColor = m_renderColors[it];

            Color vertexColor( particleColor.x, particleColor.y, particleColor.z, particleColor.w );
            vertexColor *= color;
            p.color = vertexColor.getAsARGB();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::updateVertexWM_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();
        bool identityWorldMatrix = this->isIdentityWorldMatrix();

        for( uint32_t it = 0, it_end = _verticesCount; it != it_end; ++it )
        {
            RenderVertex2D & r = _vertices[it];

            r.position.z = -r.position.z;

            if( identityWorldMatrix == true )
            {
                continue;
            }

            mt::vec3f wm_pos;
            mt::mul_v3_v3_m4( &wm_pos, r.position, wm );

            r.position = wm_pos;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffect::updateVertexUV_( RenderVertex2D * const _vertices, uint32_t _verticesCount ) const
    {
        if( m_resourceImage == nullptr )
        {
            return;
        }

        for( uint32_t it = 0, it_end = _verticesCount; it != it_end; ++it )
        {
            RenderVertex2D & r = _vertices[it];

            m_resourceImage->correctUV( 0, r.uv[0], &(r.uv[0]) );
            m_resourceImage->correctUV( 1, r.uv[1], &( r.uv[1] ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
