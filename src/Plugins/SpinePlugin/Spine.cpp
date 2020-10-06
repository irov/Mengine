#include "Spine.h"

#include "Interface/TimelineServiceInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"

#include "stdex/memorycopy.h"

#include <algorithm>

#ifndef MENGINE_SPINE_MAX_VERTICES
#define MENGINE_SPINE_MAX_VERTICES 512
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Spine::Spine()
        : m_skeleton( nullptr )
        , m_animationStateData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Spine::~Spine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton )
    {
        if( m_resourceSpineSkeleton == _resourceSpineSkeleton )
        {
            return;
        }

        this->recompile( [this, &_resourceSpineSkeleton]()
        {
            m_resourceSpineSkeleton = _resourceSpineSkeleton;

            if( m_resourceSpineSkeleton == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Spine::getResourceSpineSkeleton() const
    {
        return m_resourceSpineSkeleton;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::addAnimationSampler( const SamplerSpineAnimationInterfacePtr & _sampler )
    {
        SamplerSpineAnimationPtr sampler = _sampler;

        m_samplers.emplace_back( sampler );

        if( this->isCompile() == false )
        {
            return true;
        }

        if( sampler->compile() == false )
        {
            LOGGER_ERROR( "spine '%s' invalid compile sampler '%s'"
                , this->getName().c_str()
                , sampler->getName().c_str()
            );

            return false;
        }

        if( sampler->getAnimationEnable() == true )
        {
            spAnimationState * animationState = sampler->getAnimationState();
            spAnimationState_apply( animationState, m_skeleton );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::removeAnimationSampler( const ConstString & _samplerName )
    {
        VectorSamplerAnimations::iterator it_found = std::find_if( m_samplers.begin(), m_samplers.end()
            , [_samplerName]( const SamplerSpineAnimationPtr & _sampler )
        {
            return _sampler->getName() == _samplerName;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_samplers.end() );

        if( this->isCompile() == true )
        {
            const SamplerSpineAnimationPtr & sampler = *it_found;

            sampler->release();
        }

        m_samplers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerSpineAnimationInterfacePtr & Spine::findAnimationSampler( const ConstString & _name ) const
    {
        for( const SamplerSpineAnimationPtr & sampler : m_samplers )
        {
            if( sampler->getName() != _name )
            {
                continue;
            }

            return sampler;
        }

        return SamplerSpineAnimationInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Spine::getAnimationSamplerCount() const
    {
        VectorSamplerAnimations::size_type count = m_samplers.size();

        return (uint32_t)count;
    }
    //////////////////////////////////////////////////////////////////////////
    const SamplerSpineAnimationInterfacePtr & Spine::getAnimationSampler( uint32_t _index ) const
    {
        const SamplerSpineAnimationPtr & sampler = m_samplers[_index];

        return sampler;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool Spine::getWorldBoundingBox( mt::box2f * _box ) const
    {
        if( this->isCompile() == false )
        {
            return false;
        }

        mt::box2f bb;
        mt::insideout_box( bb );

        int slotCount = m_skeleton->slotsCount;

        const RenderIndex quadTriangles[6] = {0, 1, 2, 2, 3, 0};

        mt::vec2f attachment_vertices[MENGINE_SPINE_MAX_VERTICES];

        for( int index_slot = 0; index_slot != slotCount; ++index_slot )
        {
            spSlot * slot = m_skeleton->drawOrder[index_slot];

            if( slot->attachment == nullptr )
            {
                continue;
            }

            int verticesCount;

            const spAttachmentType attachment_type = slot->attachment->type;

            switch( attachment_type )
            {
            case SP_ATTACHMENT_REGION:
                {
                    spRegionAttachment * const attachment = (spRegionAttachment * const)slot->attachment;

                    spRegionAttachment_computeWorldVertices( attachment, slot->bone, (float *)attachment_vertices, 0, 2 );

                    verticesCount = 4;
                }break;
            case SP_ATTACHMENT_MESH:
                {
                    spMeshAttachment * const attachment = (spMeshAttachment * const)slot->attachment;

                    spVertexAttachment_computeWorldVertices( &attachment->super, slot, 0, attachment->super.worldVerticesLength, (float *)attachment_vertices, 0, 2 );

                    verticesCount = attachment->super.worldVerticesLength;
                }break;
            default:
                continue;
                break;
            }

            for( int index_vertex = 0; index_vertex != verticesCount; ++index_vertex )
            {
                const mt::vec2f & v = attachment_vertices[index_vertex];

                mt::add_internal_point( bb, v );
            }
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mul_box2_m4( *_box, bb, wm );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceSpineSkeleton, "'%s' resource is null"
            , m_name.c_str()
        );

        if( m_resourceSpineSkeleton->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceSpineSkeleton->getName().c_str()
            );

            return false;
        }

        spSkeletonData * skeletonData = m_resourceSpineSkeleton->getSkeletonData();

        spAnimationStateData * animationStateData = spAnimationStateData_create( skeletonData );
        MENGINE_ASSERTION_MEMORY_PANIC( animationStateData );

        m_animationStateData = animationStateData;

        spSkeleton * skeleton = spSkeleton_create( skeletonData );
        MENGINE_ASSERTION_MEMORY_PANIC( skeleton );

        m_skeleton = skeleton;

        spSkeleton_setToSetupPose( m_skeleton );

        for( const SamplerSpineAnimationPtr & sampler : m_samplers )
        {
            if( sampler->compile() == false )
            {
                LOGGER_ERROR( "spine '%s' invalid compile sampler '%s'"
                    , this->getName().c_str()
                    , sampler->getName().c_str()
                );

                return false;
            }

            spAnimationState * animationState = sampler->getAnimationState();
            spAnimationState_apply( animationState, m_skeleton );
        }

        spSkeleton_updateCache( m_skeleton );
        spSkeleton_updateWorldTransform( m_skeleton );

        int slotCount = m_skeleton->slotsCount;

        for( int index = 0; index != slotCount; ++index )
        {
            spSlot * slot = m_skeleton->drawOrder[index];

            uint32_t slotIndex = (uint32_t)slot->data->index;

            VectorAttachmentMesh::const_iterator it_found = std::find_if( m_attachmentMeshes.begin(), m_attachmentMeshes.end(), [slotIndex]( const AttachmentMeshDesc & _desc )
            {
                return _desc.index == slotIndex;
            } );

            if( it_found != m_attachmentMeshes.end() )
            {
                continue;
            }

            AttachmentMeshDesc desc;
            desc.index = slotIndex;
            desc.image = nullptr;

            m_attachmentMeshes.emplace_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_release()
    {
        Node::_release();

        for( const SamplerSpineAnimationPtr & sampler : m_samplers )
        {
            sampler->release();
        }

        if( m_skeleton != nullptr )
        {
            spSkeleton_dispose( m_skeleton );
            m_skeleton = nullptr;
        }

        if( m_animationStateData != nullptr )
        {
            spAnimationStateData_dispose( m_animationStateData );
            m_animationStateData = nullptr;
        }

        m_attachmentMeshes.clear();

        m_resourceSpineSkeleton->release();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Spine::makeMaterial_( spSlot * _slot, const ResourceImage * _resourceImage ) const
    {
        EMaterialBlendMode blendMode = EMB_NORMAL;

        const spSlotData * data = _slot->data;

        switch( data->blendMode )
        {
        case SP_BLEND_MODE_NORMAL:
            {
                blendMode = EMB_NORMAL;
            }break;
        case SP_BLEND_MODE_ADDITIVE:
            {
                blendMode = EMB_ADD;
            }break;
        case SP_BLEND_MODE_MULTIPLY:
            {
                blendMode = EMB_MULTIPLY;
            }break;
        case SP_BLEND_MODE_SCREEN:
            {
                blendMode = EMB_SCREEN;
            }break;
        default:
            break;
        }

        RenderMaterialInterfacePtr material = Helper::makeImageMaterial( ResourceImagePtr::from( _resourceImage ), ConstString::none(), blendMode, false, false, MENGINE_DOCUMENT_FORWARD );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' resource '%s' image '%s' m_material is NULL"
            , this->getName().c_str()
            , m_resourceSpineSkeleton->getName().c_str()
            , _resourceImage->getName().c_str()
        );

        return material;
    }    
    //////////////////////////////////////////////////////////////////////////
    void Spine::update( const UpdateContext * _context )
    {
        if( m_samplers.empty() == true )
        {
            return;
        }

        float spTime = _context->time * 0.001f;

        spSkeleton_update( m_skeleton, spTime );

        for( const SamplerSpineAnimationPtr & sampler : m_samplers )
        {
            if( sampler->getAnimationEnable() == false )
            {
                continue;
            }

            sampler->update( _context );

            spAnimationState * animationState = sampler->getAnimationState();
            spAnimationState_apply( animationState, m_skeleton );
        }

        spSkeleton_updateWorldTransform( m_skeleton );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        int slotCount = m_skeleton->slotsCount;

        const mt::mat4f & wm = this->getWorldMatrix();

        const RenderIndex quadTriangles[6] = {0, 1, 2, 2, 3, 0};

        float attachment_vertices[MENGINE_SPINE_MAX_VERTICES * 2];

        Color color;
        this->calcTotalColor( &color );

        float nr = color.getR();
        float ng = color.getG();
        float nb = color.getB();
        float na = color.getA();

        for( int index = 0; index != slotCount; ++index )
        {
            spSlot * slot = m_skeleton->drawOrder[index];

            if( slot->attachment == nullptr )
            {
                continue;
            }

            uint32_t slotIndex = (uint32_t)slot->data->index;

            VectorAttachmentMesh::iterator it_found = std::find_if( m_attachmentMeshes.begin(), m_attachmentMeshes.end(), [slotIndex]( const AttachmentMeshDesc & _desc )
            {
                return _desc.index == slotIndex;
            } );

            MENGINE_ASSERTION_FATAL( it_found != m_attachmentMeshes.end() );

            AttachmentMeshDesc & mesh = *it_found;

            const float * uvs = nullptr;
            int verticesCount;
            const RenderIndex * triangles;
            int trianglesCount;

            float ar = 1.f;
            float ag = 1.f;
            float ab = 1.f;
            float aa = 1.f;

            const spAttachmentType attachment_type = slot->attachment->type;

            const ResourceImage * resourceImage = nullptr;

            switch( attachment_type )
            {
            case SP_ATTACHMENT_REGION:
                {
                    spRegionAttachment * attachment = (spRegionAttachment *)slot->attachment;

                    spRegionAttachment_computeWorldVertices( attachment, slot->bone, attachment_vertices, 0, 2 );

                    uvs = attachment->uvs;
                    verticesCount = 4;
                    triangles = quadTriangles;
                    trianglesCount = 6;
                    ar = attachment->color.r;
                    ag = attachment->color.g;
                    ab = attachment->color.b;
                    aa = attachment->color.a;

                    resourceImage = static_cast<const ResourceImage *>(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

                    mesh.vertices.resize( 4 );
                    mesh.indices.resize( 6 );
                }break;
            case SP_ATTACHMENT_MESH:
                {
                    spMeshAttachment * attachment = (spMeshAttachment *)slot->attachment;

                    spVertexAttachment_computeWorldVertices( &attachment->super, slot, 0, attachment->super.worldVerticesLength, attachment_vertices, 0, 2 );

                    uvs = attachment->uvs;
                    verticesCount = attachment->super.worldVerticesLength;
                    triangles = attachment->triangles;
                    trianglesCount = attachment->trianglesCount;
                    ar = attachment->color.r;
                    ag = attachment->color.g;
                    ab = attachment->color.b;
                    aa = attachment->color.a;

                    resourceImage = static_cast<const ResourceImage *>(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

                    mesh.vertices.resize( verticesCount );
                    mesh.indices.resize( trianglesCount );
                }break;
            default:
                continue;
                break;
            }

            if( mesh.image != resourceImage )
            {
                mesh.image = resourceImage;

                mesh.material = this->makeMaterial_( slot, resourceImage );
            }

            float wr = nr * m_skeleton->color.r * slot->color.r * ar;
            float wg = ng * m_skeleton->color.g * slot->color.g * ag;
            float wb = nb * m_skeleton->color.b * slot->color.b * ab;
            float wa = na * m_skeleton->color.a * slot->color.a * aa;

            ColorValue_ARGB argb = Helper::makeRGBA( wr, wg, wb, wa );

            RenderVertex2D * vertices = mesh.vertices.data();
            RenderIndex * indices = mesh.indices.data();

            this->fillVertices_( vertices, attachment_vertices, uvs, argb, verticesCount, wm );
            this->fillIndices_( indices, triangles, trianglesCount );

            const RenderMaterialInterfacePtr & material = mesh.material;

            const mt::box2f * bb = this->getBoundingBox();

            _renderPipeline->addRenderObject( _context, material, nullptr, vertices, verticesCount, indices, trianglesCount, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColorValue_ARGB _argb, int _count, const mt::mat4f & _wm ) const
    {
        for( int i = 0; i != _count; ++i )
        {
            int index_x = i * 2 + 0;
            int index_y = i * 2 + 1;

            mt::vec2f v;
            v.x = _vertices[index_x];
            v.y = -_vertices[index_y];

            RenderVertex2D & vertex = _vertices2D[i];

            mt::mul_v3_v2_m4( vertex.position, v, _wm );

            float uv_x = _uv[index_x];
            float uv_y = _uv[index_y];

            for( int j = 0; j != MENGINE_RENDER_VERTEX_UV_COUNT; ++j )
            {
                vertex.uv[j].x = uv_x;
                vertex.uv[j].y = uv_y;
            }

            vertex.color = _argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::fillIndices_( RenderIndex * _indices, const RenderIndex * _triangles, uint32_t _count ) const
    {
        stdex::memorycopy_pod( _indices, 0, _triangles, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        for( const SamplerSpineAnimationPtr & sampler : m_samplers )
        {
            bool autoPlay = sampler->isAutoPlay();

            if( autoPlay == true )
            {
                float time = TIMELINE_SERVICE()
                    ->getTotalTime();

                if( sampler->play( time ) == 0 )
                {
                    LOGGER_ERROR( "spine '%s' sampler '%s' resource '%s' auto play return 0"
                        , this->getName().c_str()
                        , sampler->getName().c_str()
                        , m_resourceSpineSkeleton->getName().c_str()
                    );

                    return false;
                }
            }
        }

        return true;
    }
}