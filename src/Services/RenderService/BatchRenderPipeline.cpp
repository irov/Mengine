#include "BatchRenderPipeline.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/ContentHelper.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ERenderObjectFlag
    {
        RENDER_OBJECT_FLAG_NONE = 0x00000000,
        RENDER_OBJECT_FLAG_DEBUG = 0x00000001
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderPassFlag
    {
        RENDER_PASS_FLAG_NONE = 0x00000000,
        RENDER_PASS_FLAG_SINGLE = 0x00000001,
        RENDER_PASS_FLAG_EXTERNAL = 0x00000002
    };
    //////////////////////////////////////////////////////////////////////////
    BatchRenderPipeline::BatchRenderPipeline()
        : m_renderService( nullptr )
        , m_batchMode( ERBM_NORMAL )
#if defined(MENGINE_DEBUG)
        , m_debugFillrateCalcMode( false )
        , m_debugStepRenderMode( false )
        , m_debugStopRenderObjects( false )
        , m_debugLimitRenderObjects( 0 )
        , m_iterateRenderObjects( 0 )
        , m_showLogRenderObjects( ~0U )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BatchRenderPipeline::~BatchRenderPipeline()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::initialize()
    {
        uint32_t Engine_RenderMaxObject = CONFIG_VALUE( "Engine", "RenderMaxObject", 8000U );
        uint32_t Engine_RenderMaxPass = CONFIG_VALUE( "Engine", "RenderMaxPass", 200U );

        m_renderObjects.reserve( Engine_RenderMaxObject );
        m_renderPrimitives.reserve( Engine_RenderMaxObject );
        m_renderPasses.reserve( Engine_RenderMaxPass );

        uint32_t Engine_RenderMaxQuadBatch = CONFIG_VALUE( "Engine", "RenderMaxQuadBatch", 2000U );
        uint32_t Engine_RenderMaxLineBatch = CONFIG_VALUE( "Engine", "RenderMaxLineBatch", 4000U );

        m_indicesQuad.resize( Engine_RenderMaxQuadBatch * 6 );
        m_indicesLine.resize( Engine_RenderMaxLineBatch * 1 );

        for( uint32_t i = 0; i != Engine_RenderMaxQuadBatch; ++i )
        {
            uint32_t indexOffset = i * 6;

            RenderIndex vertexOffset = (RenderIndex)i * 4;

            m_indicesQuad[indexOffset + 0] = vertexOffset + 0;
            m_indicesQuad[indexOffset + 1] = vertexOffset + 3;
            m_indicesQuad[indexOffset + 2] = vertexOffset + 1;
            m_indicesQuad[indexOffset + 3] = vertexOffset + 1;
            m_indicesQuad[indexOffset + 4] = vertexOffset + 3;
            m_indicesQuad[indexOffset + 5] = vertexOffset + 2;
        }

        for( uint32_t i = 0; i != Engine_RenderMaxLineBatch; ++i )
        {
            RenderIndex vertexOffset = (RenderIndex)i;

            m_indicesLine[i] = vertexOffset;
        }

        uint32_t Engine_RenderServiceBatchMode = CONFIG_VALUE( "Engine", "RenderServiceBatchMode", 1 );

        switch( Engine_RenderServiceBatchMode )
        {
        case 0U:
            {
                m_batchMode = ERBM_NONE;
            }break;
        case 1U:
            {
                m_batchMode = ERBM_NORMAL;
            }break;
        case 2U:
            {
                m_batchMode = ERBM_SMART;
            }break;
        default:
            {
                m_batchMode = ERBM_SMART;
            }break;
        }

        m_renderService = RENDER_SERVICE();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::finalize()
    {
        for( RenderPrimitive & rp : m_renderPrimitives )
        {
            IntrusivePtrBase::intrusive_ptr_release( rp.material );
        }

        m_renderPasses.clear();

        m_renderObjects.clear();
        m_renderPrimitives.clear();

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        m_debugRenderObjects.clear();
#endif

        m_indicesQuad.clear();
        m_indicesLine.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addRenderMesh( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderProgramVariableInterfacePtr & _programVariable
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , uint32_t _vertexCount, uint32_t _indexCount
        , uint32_t _baseVertexIndex, uint32_t _startIndex, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _context != nullptr, "context == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->viewport != nullptr, "_context->viewport == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->camera != nullptr, "_context->camera == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _material != nullptr, "_material == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _vertexBuffer != nullptr, "_vertexBuffer == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _indexBuffer != nullptr, "_indexBuffer == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );

        if( m_renderObjects.full() == true )
        {
            LOGGER_ASSERTION( "max render objects %u"
                , m_renderObjects.size()
            );

            return;
        }

#if defined(MENGINE_DEBUG)
        if( m_debugStepRenderMode == true /*&& _debug == false*/ )
        {
            if( m_iterateRenderObjects++ >= m_debugLimitRenderObjects && m_debugLimitRenderObjects > 0 && m_debugStopRenderObjects == false )
            {
                return;
            }
        }
#endif

        const RenderMaterialStage * materialStage = _material->getStage();

        const RenderProgramInterfacePtr & program = materialStage->program;

        const RenderVertexAttributeInterfacePtr & vertexAttribute = program->getVertexAttribute();

        RenderPass & renderPass = this->requestRenderPass_( _context, nullptr, _vertexBuffer, _indexBuffer, vertexAttribute, _programVariable );

        renderPass.flags |= RENDER_PASS_FLAG_SINGLE;

        ++renderPass.countRenderObject;

        RenderPrimitive & rp = m_renderPrimitives.emplace();

        IntrusivePtrBase::intrusive_ptr_setup( rp.material, _material.get() );

        rp.baseVertexIndex = _baseVertexIndex;
        rp.minIndex = 0;
        rp.vertexCount = _vertexCount;
        rp.startIndex = _startIndex;
        rp.indexCount = _indexCount;

        RenderObject & ro = m_renderObjects.emplace();

        ro.vertexData = nullptr;
        ro.vertexCount = 0;

        ro.indexData = nullptr;
        ro.indexCount = 0;

        if( m_batchMode == ERBM_SMART )
        {
            ro.materialSmartId = 0;

            mt::box2f bb;
            bb.minimum.x = (std::numeric_limits<float>::lowest)();
            bb.minimum.y = (std::numeric_limits<float>::lowest)();
            bb.maximum.x = (std::numeric_limits<float>::max)();
            bb.maximum.y = (std::numeric_limits<float>::max)();

            ro.bb = bb;
        }

        ro.flags = RENDER_OBJECT_FLAG_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addRenderObject( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderProgramVariableInterfacePtr & _programVariable
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const RenderIndex * _indices, uint32_t _indexCount
        , const mt::box2f * _bb, bool _debug, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _context != nullptr, "context == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->viewport != nullptr, "_context->viewport == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->camera != nullptr, "_context->camera == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _material != nullptr, "_material == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _vertices != nullptr, "_vertices == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _indices != nullptr, "_indices == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );

        if( _vertexCount >= MENGINE_RENDER_VERTEX_MAX_BATCH )
        {
            LOGGER_ASSERTION( "_vertexCount(%u) >= MENGINE_RENDER_VERTEX_MAX_BATCH(%u)"
                , _vertexCount
                , MENGINE_RENDER_VERTEX_MAX_BATCH
            );

            return;
        }

        if( m_renderObjects.full() == true )
        {
            LOGGER_ASSERTION( "max render objects '%u'"
                , m_renderObjects.size()
            );

            return;
        }

#if defined(MENGINE_DEBUG)
        if( m_debugStepRenderMode == true /*&& _debug == false*/ )
        {
            if( m_iterateRenderObjects++ >= m_debugLimitRenderObjects && m_debugLimitRenderObjects > 0 && m_debugStopRenderObjects == false )
            {
                return;
            }
        }
#endif

        const RenderMaterialStage * materialStage = _material->getStage();

        const RenderProgramInterfacePtr & program = materialStage->program;

        const RenderVertexAttributeInterfacePtr & vertexAttribute = program->getVertexAttribute();

        const RenderBatchInterfacePtr & batch = m_renderService->requestRenderBatch( vertexAttribute, _vertexCount, _indexCount, _doc );

        const RenderVertexBufferInterfacePtr & vertexBuffer = batch->getVertexBuffer();
        const RenderIndexBufferInterfacePtr & indexBuffer = batch->getIndexBuffer();

        RenderPass & renderPass = this->requestRenderPass_( _context, batch, vertexBuffer, indexBuffer, vertexAttribute, _programVariable );

#if defined(MENGINE_STATISTIC_ENABLE)
        if( _debug == false )
        {
            EPrimitiveType primitiveType = _material->getPrimitiveType();

            switch( primitiveType )
            {
            case PT_TRIANGLELIST:
                {
                    const RenderViewportInterface * viewport = renderPass.context.viewport;
                    const Viewport & vp = viewport->getViewport();

                    double fillrate = Helper::calcRenderQuadSquare( _vertices, _vertexCount, vp );

                    STATISTIC_ADD_DOUBLE( STATISTIC_RENDER_PERFRAME_FILLRATE, fillrate );
                }break;
            default:
                {
                }break;
            }
        }
#endif

        ++renderPass.countRenderObject;

        RenderPrimitive & rp = m_renderPrimitives.emplace();

        RenderMaterialInterfacePtr rp_material = _material;

#if defined(MENGINE_DEBUG)
        if( m_debugStepRenderMode == true && _debug == false )
        {
            if( m_iterateRenderObjects == m_debugLimitRenderObjects && m_debugLimitRenderObjects > 0 && m_debugStopRenderObjects == false )
            {
                const RenderTextureInterfacePtr & texture = _material->getTexture( 0 );

                if( texture != nullptr )
                {
                    if( m_showLogRenderObjects != m_debugLimitRenderObjects )
                    {
                        LOGGER_MESSAGE_RELEASE( "index: %u texture: %s material: %s"
                            , m_debugLimitRenderObjects
                            , Helper::getContentFullPath( texture->getContent() ).c_str()
                            , _material->getName().c_str()
                        );

                        m_showLogRenderObjects = m_debugLimitRenderObjects;
                    }
                }

                EPrimitiveType primitiveType = _material->getPrimitiveType();

                const RenderMaterialInterfacePtr & new_material = RENDERMATERIAL_SERVICE()
                    ->getMaterial3( EM_COLOR_BLEND, primitiveType, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

                if( new_material == nullptr )
                {
                    return;
                }

                rp_material = new_material;
            }
        }
#endif

        IntrusivePtrBase::intrusive_ptr_setup( rp.material, rp_material.get() );

        rp.minIndex = 0;
        rp.startIndex = 0;
        rp.baseVertexIndex = 0;

        rp.vertexCount = 0;
        rp.indexCount = 0;

        RenderObject & ro = m_renderObjects.emplace();

        ro.vertexData = _vertices;
        ro.vertexCount = _vertexCount;

        ro.indexData = _indices;
        ro.indexCount = _indexCount;

        if( m_batchMode == ERBM_SMART )
        {
            UniqueId materialId = rp.material->getId();
            ro.materialSmartId = materialId % MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX;

            renderPass.materialEnd[ro.materialSmartId] = &ro;

            mt::box2f bb;

            if( _bb != nullptr )
            {
                bb = *_bb;
            }
            else
            {
                Helper::makeRenderBoundingBox( &bb, _vertices, _vertexCount );
            }

            const RenderCameraInterface * camera = renderPass.context.camera;
            const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

            const RenderViewportInterface * viewport = renderPass.context.viewport;
            const Viewport & vp = viewport->getViewport();

            mt::box2f bb_homogenize;
            mt::box2_homogenize( &bb_homogenize, bb.minimum, bb.maximum, vpm );

            mt::vec2f vp_scale;
            vp.calcSize( &vp_scale );

            mt::box2_scale( &bb_homogenize, vp_scale );
            mt::box2_transpose( &bb_homogenize, vp.begin );

            ro.bb = bb_homogenize;
        }

        ro.flags = RENDER_OBJECT_FLAG_NONE;

        if( _debug == true )
        {
            ro.flags |= RENDER_OBJECT_FLAG_DEBUG;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addRenderExternal( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderProgramVariableInterfacePtr & _programVariable
        , const RenderDrawPrimitiveInterfacePtr & _drawPrimitive
        , const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _context != nullptr, "context == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->viewport != nullptr, "_context->viewport == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->camera != nullptr, "_context->camera == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _material != nullptr, "_material == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _drawPrimitive != nullptr, "_external == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );

        if( m_renderObjects.full() == true )
        {
            LOGGER_ASSERTION( "max render objects %u"
                , m_renderObjects.size()
            );

            return;
        }

#if defined(MENGINE_DEBUG)
        if( m_debugStepRenderMode == true /*&& _debug == false*/ )
        {
            if( m_iterateRenderObjects++ >= m_debugLimitRenderObjects && m_debugLimitRenderObjects > 0 && m_debugStopRenderObjects == false )
            {
                return;
            }
        }
#endif

        const RenderMaterialStage * materialStage = _material->getStage();

        const RenderProgramInterfacePtr & program = materialStage->program;

        const RenderVertexAttributeInterfacePtr & vertexAttribute = program->getVertexAttribute();

        RenderPass renderPass;

        renderPass.beginRenderObject = 0U;
        renderPass.countRenderObject = 0U;

        renderPass.batch = nullptr;

        renderPass.vertexAttribute = vertexAttribute;
        renderPass.vertexBuffer = nullptr;
        renderPass.indexBuffer = nullptr;
        renderPass.programVariable = _programVariable;

        renderPass.context = *_context;

        renderPass.zGroup = _context->zGroup == MENGINE_RENDER_ZGROUP_DEFAULT ? 0 : _context->zGroup;
        renderPass.zIndex = _context->zIndex == MENGINE_RENDER_ZINDEX_DEFAULT ? 0 : _context->zIndex;
        
        renderPass.drawPrimitive = _drawPrimitive;

        renderPass.flags = RENDER_PASS_FLAG_SINGLE;

        m_renderPasses.emplace_back( renderPass );
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addDebugRenderObject( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const RenderIndex * _indices, uint32_t _indexCount, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _context != nullptr, "context == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->viewport != nullptr, "_context->viewport == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _context->camera != nullptr, "_context->camera == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _material != nullptr, "_material == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _vertices != nullptr, "_vertices == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );
        MENGINE_ASSERTION_FATAL( _indices != nullptr, "_indices == nullptr (doc: %s)", MENGINE_DOCUMENT_STR( _doc ) );

        if( _vertexCount >= MENGINE_RENDER_VERTEX_MAX_BATCH )
        {
            LOGGER_ASSERTION( "_vertexCount(%u) >= MENGINE_RENDER_VERTEX_MAX_BATCH(%u)"
                , _vertexCount
                , MENGINE_RENDER_VERTEX_MAX_BATCH
            );

            return;
        }

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        DebugRenderObject dro;
        dro.context = *_context;
        dro.material = _material;
        dro.vertices = _vertices;
        dro.vertexCount = _vertexCount;
        dro.indices = _indices;
        dro.indexCount = _indexCount;

#if defined(MENGINE_DOCUMENT_ENABLE)
        dro.doc = _doc;
#endif

        m_debugRenderObjects.push_back( dro );
#else
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _material );
        MENGINE_UNUSED( _vertices );
        MENGINE_UNUSED( _vertexCount );
        MENGINE_UNUSED( _indices );
        MENGINE_UNUSED( _indexCount );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addDebugRenderQuad( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount, const DocumentInterfacePtr & _doc )
    {
        uint32_t indicesCount = (_vertexCount / 4) * 6;

        if( indicesCount >= m_indicesQuad.size() )
        {
            LOGGER_ASSERTION( "count [%u] > max count [%u]"
                , indicesCount
                , m_indicesQuad.size()
            );

            return;
        }

        RenderIndex * indices = m_indicesQuad.buff();

        this->addDebugRenderObject( _context, _material, _vertices, _vertexCount, indices, indicesCount, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addRenderQuad( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug, const DocumentInterfacePtr & _doc )
    {
        uint32_t indicesCount = (_vertexCount / 4) * 6;

        if( indicesCount >= m_indicesQuad.size() )
        {
            LOGGER_ASSERTION( "count [%u] > max count [%u]"
                , indicesCount
                , m_indicesQuad.size()
            );

            return;
        }

        RenderIndex * indices = m_indicesQuad.buff();

        this->addRenderObject( _context, _material, nullptr, _vertices, _vertexCount, indices, indicesCount, _bb, _debug, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::addRenderLine( const RenderContext * _context
        , const RenderMaterialInterfacePtr & _material
        , const RenderVertex2D * _vertices, uint32_t _vertexCount
        , const mt::box2f * _bb, bool _debug, const DocumentInterfacePtr & _doc )
    {
        uint32_t indicesCount = _vertexCount;

        if( indicesCount >= m_indicesLine.size() )
        {
            LOGGER_ASSERTION( "count [%u] > max count [%u]"
                , indicesCount
                , m_indicesLine.size()
            );

            return;
        }

        RenderIndex * indices = m_indicesLine.buff();

        this->addRenderObject( _context, _material, nullptr, _vertices, _vertexCount, indices, indicesCount, _bb, _debug, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::enableDebugFillrateCalcMode( bool _enable )
    {
        MENGINE_UNUSED( _enable );

#if defined(MENGINE_DEBUG)
        m_debugFillrateCalcMode = _enable;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::isDebugFillrateCalcMode() const
    {
#if defined(MENGINE_DEBUG)
        return m_debugFillrateCalcMode;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::enableDebugStepRenderMode( bool _enable )
    {
        MENGINE_UNUSED( _enable );

#if defined(MENGINE_DEBUG)
        m_debugStepRenderMode = _enable;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::isDebugStepRenderMode() const
    {
#if defined(MENGINE_DEBUG)
        return m_debugStepRenderMode;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::endDebugLimitRenderObjects()
    {
#if defined(MENGINE_DEBUG)
        m_debugStopRenderObjects = true;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::increfDebugLimitRenderObjects()
    {
#if defined(MENGINE_DEBUG)
        ++m_debugLimitRenderObjects;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::decrefDebugLimitRenderObjects()
    {
#if defined(MENGINE_DEBUG)
        if( m_debugLimitRenderObjects == 0 )
        {
            return false;
        }

        --m_debugLimitRenderObjects;

        if( m_debugLimitRenderObjects == 0 )
        {
            m_showLogRenderObjects = ~0U;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::prepare()
    {
        m_renderPasses.clear();

        m_renderObjects.clear();
        m_renderPrimitives.clear();

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        m_debugRenderObjects.clear();
#endif

#if defined(MENGINE_DEBUG)
        m_iterateRenderObjects = 0;
        m_debugStopRenderObjects = false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::flush()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        for( const DebugRenderObject & dro : m_debugRenderObjects )
        {
            this->addRenderObject( &dro.context, dro.material, nullptr, dro.vertices, dro.vertexCount, dro.indices, dro.indexCount, nullptr, true, MENGINE_DOCUMENT_VALUE( dro.doc, nullptr ) );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::batch()
    {
        MENGINE_PROFILER_CATEGORY();

        for( const RenderPass & pass : m_renderPasses )
        {
            if( (pass.flags & RENDER_PASS_FLAG_SINGLE) == RENDER_PASS_FLAG_SINGLE )
            {
                continue;
            }

            const RenderBatchInterfacePtr & batch = pass.batch;

            RenderBatchLockData * lockData = batch->getLockData();

            this->insertRenderObjects_( pass, lockData->vertexMemory, lockData->vertexSize, lockData->indexMemory, &lockData->vbPos, &lockData->ibPos );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::render()
    {
        MENGINE_PROFILER_CATEGORY();

        Algorithm::stable_sort( m_renderPasses.begin(), m_renderPasses.end(), []( const RenderPass & _l, const RenderPass & _r )
        {
            if( _l.zGroup < _r.zGroup )
            {
                return true;
            }
            else if( _l.zGroup > _r.zGroup )
            {
                return false;
            }

            return _l.zIndex < _r.zIndex;
        } );

        const RenderPrimitive * renderPrimitives = m_renderPrimitives.buff();

        for( const RenderPass & renderPass : m_renderPasses )
        {
            const RenderVertexBufferInterfacePtr & vertexBuffer = renderPass.vertexBuffer;
            const RenderIndexBufferInterfacePtr & indexBuffer = renderPass.indexBuffer;

            const RenderContext * context = &renderPass.context;

            const RenderProgramVariableInterfacePtr & programVariable = renderPass.programVariable;
            const RenderDrawPrimitiveInterfacePtr & drawPrimitive = renderPass.drawPrimitive;

            m_renderService->beginRenderPass( vertexBuffer, indexBuffer, programVariable, context );

            if( drawPrimitive == nullptr )
            {
                m_renderService->drawPrimitives( renderPrimitives + renderPass.beginRenderObject, renderPass.countRenderObject );
            }
            else
            {
                drawPrimitive->onRenderDrawPrimitives( renderPrimitives + renderPass.beginRenderObject, renderPass.countRenderObject );
            }

            m_renderService->endRenderPass( context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::clear()
    {
        m_renderPrimitives.clear();

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        m_debugRenderObjects.clear();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::isEmpty() const
    {
        bool empty = m_renderPrimitives.empty();

        return empty;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::insertRenderObjects_( const RenderPass & _renderPass, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t * const _vbPos, uint32_t * const _ibPos )
    {
        uint32_t vbPos = *_vbPos;
        uint32_t ibPos = *_ibPos;

        DynamicArrayRenderObjects::iterator it_object = m_renderObjects.advance( _renderPass.beginRenderObject );
        DynamicArrayRenderObjects::iterator it_object_end = m_renderObjects.advance( _renderPass.beginRenderObject + _renderPass.countRenderObject );

        DynamicArrayRenderPrimitives::iterator it_primitive = m_renderPrimitives.advance( _renderPass.beginRenderObject );

        for( ; it_object != it_object_end; ++it_object, ++it_primitive )
        {
            RenderObject * ro = it_object;
            RenderPrimitive * rp = it_primitive;

            if( ro->vertexCount == 0 )
            {
                continue;
            }

            rp->startIndex = ibPos;
            rp->minIndex = vbPos;

            if( this->insertRenderObject_( ro, _vertexMemory, _vertexSize, _indexMemory, vbPos, ibPos ) == false )
            {
                break;
            }

            rp->vertexCount = ro->vertexCount;
            rp->indexCount = ro->indexCount;

            vbPos += ro->vertexCount;
            ibPos += ro->indexCount;

            ro->vertexCount = 0;
            ro->indexCount = 0;

            switch( m_batchMode )
            {
            case ERBM_NONE:
                {
                }break;
            case ERBM_NORMAL:
                {
                    this->batchRenderObjectNormal_( it_object, it_object_end, it_primitive, rp, _vertexMemory, _vertexSize, _indexMemory, &vbPos, &ibPos );
                }break;
            case ERBM_SMART:
                {
                    this->batchRenderObjectNormal_( it_object, it_object_end, it_primitive, rp, _vertexMemory, _vertexSize, _indexMemory, &vbPos, &ibPos );

                    this->batchRenderObjectSmart_( _renderPass, it_object, it_primitive, ro, rp, _vertexMemory, _vertexSize, _indexMemory, &vbPos, &ibPos );
                }break;
            }
        }

        *_vbPos = vbPos;
        *_ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::batchRenderObjectNormal_( DynamicArrayRenderObjects::iterator _roBegin, DynamicArrayRenderObjects::iterator _roEnd, DynamicArrayRenderPrimitives::iterator _rpBegin, RenderPrimitive * const _rp, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t * const _vbPos, uint32_t * const _ibPos )
    {
        uint32_t vbPos = *_vbPos;
        uint32_t ibPos = *_ibPos;

        DynamicArrayRenderObjects::iterator it_ro_batch_begin = _roBegin;
        ++it_ro_batch_begin;

        DynamicArrayRenderPrimitives::iterator it_rp_batch_begin = _rpBegin;
        ++it_rp_batch_begin;

        RenderMaterialInterface * rp_material = _rp->material;

        for( ; it_ro_batch_begin != _roEnd; ++it_ro_batch_begin, ++it_rp_batch_begin )
        {
            RenderObject * ro_bath_begin = it_ro_batch_begin;
            RenderPrimitive * rp_bath_begin = it_rp_batch_begin;

            if( ro_bath_begin->indexCount == 0 )
            {
                continue;
            }

            RenderMaterialInterface * batch_material = rp_bath_begin->material;

            if( rp_material != batch_material )
            {
                break;
            }

            if( this->insertRenderObject_( ro_bath_begin, _vertexMemory, _vertexSize, _indexMemory, vbPos, ibPos ) == false )
            {
                break;
            }

            vbPos += ro_bath_begin->vertexCount;
            ibPos += ro_bath_begin->indexCount;

            _rp->vertexCount += ro_bath_begin->vertexCount;
            _rp->indexCount += ro_bath_begin->indexCount;

            IntrusivePtrBase::intrusive_ptr_release( rp_bath_begin->material );
            rp_bath_begin->material = nullptr;

            rp_bath_begin->vertexCount = 0;
            rp_bath_begin->indexCount = 0;
            ro_bath_begin->vertexCount = 0;
            ro_bath_begin->indexCount = 0;
        }

        *_vbPos = vbPos;
        *_ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE static bool s_testRenderBB( const RenderObject * _begin, const RenderObject * _end, const RenderObject * _ro )
    {
        for( const RenderObject * it = _begin; it != _end; ++it )
        {
            const RenderObject * ro_bath = it;

            if( ro_bath->vertexCount == 0 )
            {
                continue;
            }

            if( mt::box2_intersect( ro_bath->bb, _ro->bb ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void BatchRenderPipeline::batchRenderObjectSmart_( const RenderPass & _renderPass, DynamicArrayRenderObjects::iterator _roBegin, DynamicArrayRenderPrimitives::iterator _rpBegin, const RenderObject * _ro, RenderPrimitive * _rp, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t * _vbPos, uint32_t * _ibPos )
    {
        uint32_t vbPos = *_vbPos;
        uint32_t ibPos = *_ibPos;

        DynamicArrayRenderObjects::iterator it_ro_batch_start_end = _roBegin;
        ++it_ro_batch_start_end;

        DynamicArrayRenderObjects::iterator it_ro_batch = _roBegin;
        ++it_ro_batch;

        DynamicArrayRenderPrimitives::iterator it_rp_batch = _rpBegin;
        ++it_rp_batch;

        uint32_t materialSmartId = _ro->materialSmartId;
        DynamicArrayRenderObjects::const_iterator it_ro_end = _renderPass.materialEnd[materialSmartId];

        if( _roBegin == it_ro_end )
        {
            return;
        }

        const RenderMaterialInterface * rp_material = _rp->material;

        for( ; it_ro_batch != it_ro_end; ++it_ro_batch, ++it_rp_batch )
        {
            RenderObject * ro_bath = it_ro_batch;
            RenderPrimitive * rp_bath = it_rp_batch;

            if( ro_bath->vertexCount == 0 )
            {
                continue;
            }

            const RenderMaterialInterface * rp_bath_material = rp_bath->material;

            if( rp_material != rp_bath_material )
            {
                continue;
            }

            if( s_testRenderBB( it_ro_batch_start_end, it_ro_batch, ro_bath ) == true )
            {
                break;
            }

            if( this->insertRenderObject_( ro_bath, _vertexMemory, _vertexSize, _indexMemory, vbPos, ibPos ) == false )
            {
                break;
            }

            _rp->vertexCount += ro_bath->vertexCount;
            _rp->indexCount += ro_bath->indexCount;

            IntrusivePtrBase::intrusive_ptr_release( rp_bath->material );
            rp_bath->material = nullptr;

            rp_bath->vertexCount = 0;
            rp_bath->indexCount = 0;

            vbPos += ro_bath->vertexCount;
            ibPos += ro_bath->indexCount;

            ro_bath->vertexCount = 0;
            ro_bath->indexCount = 0;
        }

        *_vbPos = vbPos;
        *_ibPos = ibPos;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::insertRenderObject_( const RenderObject * _renderObject, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t _vbPos, uint32_t _ibPos ) const
    {
        void * vertexBuffer = _vertexMemory->getBuffer();
        size_t vertexBufferSize = _vertexMemory->getSize();

        size_t vertexBufferOffset = _vbPos * _vertexSize;
        size_t vertexCountSize = _renderObject->vertexCount * _vertexSize;

        if( vertexBufferOffset + vertexCountSize > vertexBufferSize )
        {
            LOGGER_ASSERTION( "vertex buffer overrlow!" );

            return false;
        }

        RenderIndex * indexBuffer = _indexMemory->getBuffer();
        size_t indexBufferSize = _indexMemory->getSize();

        size_t indexBufferOffset = _ibPos * sizeof( RenderIndex );
        size_t indexCountSize = _renderObject->indexCount * sizeof( RenderIndex );

        if( indexBufferOffset + indexCountSize > indexBufferSize )
        {
            LOGGER_ASSERTION( "index buffer overrlow!" );

            return false;
        }

        Helper::memoryCopy( vertexBuffer, vertexBufferOffset, _renderObject->vertexData, 0, vertexCountSize );

        RenderIndex * offsetIndicesBuffer = indexBuffer + _ibPos;

        RenderIndex * src = offsetIndicesBuffer;
        RenderIndex * src_end = offsetIndicesBuffer + _renderObject->indexCount;
        const RenderIndex * dst = _renderObject->indexData;

        RenderIndex indices_offset = (RenderIndex)_vbPos;
        while( src != src_end )
        {
            *src = *dst + indices_offset;
            ++src;
            ++dst;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BatchRenderPipeline::testRenderPass_( const RenderContext * _context
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , const RenderVertexAttributeInterfacePtr & _vertexAttribute
        , const RenderProgramVariableInterfacePtr & _programVariable ) const
    {
        if( m_renderPasses.empty() == true )
        {
            return true;
        }

        const RenderPass & pass = m_renderPasses.back();

        if( pass.vertexBuffer != _vertexBuffer ||
            pass.indexBuffer != _indexBuffer ||
            pass.vertexAttribute != _vertexAttribute ||
            pass.programVariable != _programVariable )
        {
            return true;
        }

        if( Helper::equalRenderContext( &pass.context, _context ) == false )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderPass & BatchRenderPipeline::requestRenderPass_( const RenderContext * _context
        , const RenderBatchInterfacePtr & _batch
        , const RenderVertexBufferInterfacePtr & _vertexBuffer
        , const RenderIndexBufferInterfacePtr & _indexBuffer
        , const RenderVertexAttributeInterfacePtr & _vertexAttribute
        , const RenderProgramVariableInterfacePtr & _programVariable )
    {
        if( this->testRenderPass_( _context, _vertexBuffer, _indexBuffer, _vertexAttribute, _programVariable ) == true )
        {
            RenderPass renderPass;

            renderPass.beginRenderObject = (uint32_t)m_renderObjects.size();
            renderPass.countRenderObject = 0U;

            renderPass.batch = _batch;

            renderPass.vertexAttribute = _vertexAttribute;
            renderPass.vertexBuffer = _vertexBuffer;
            renderPass.indexBuffer = _indexBuffer;
            renderPass.programVariable = _programVariable;
            
            renderPass.context = *_context;

            renderPass.zGroup = _context->zGroup == MENGINE_RENDER_ZGROUP_DEFAULT ? 0 : _context->zGroup;
            renderPass.zIndex = _context->zIndex == MENGINE_RENDER_ZINDEX_DEFAULT ? 0 : _context->zIndex;
            
            renderPass.flags = RENDER_PASS_FLAG_NONE;

            m_renderPasses.emplace_back( renderPass );
        }

        RenderPass & rp = m_renderPasses.back();

        return rp;
    }
    //////////////////////////////////////////////////////////////////////////
}