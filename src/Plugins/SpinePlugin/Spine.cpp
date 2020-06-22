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
    bool Spine::mixAnimation( const ConstString & _first, const ConstString & _second, float _mix )
    {
        if( m_resourceSpineSkeleton == nullptr )
        {
            return false;
        }

        spAnimation * animation_first = m_resourceSpineSkeleton->findSkeletonAnimation( _first );

        if( animation_first == nullptr )
        {
            return false;
        }

        spAnimation * animation_second = m_resourceSpineSkeleton->findSkeletonAnimation( _second );

        if( animation_second == nullptr )
        {
            return false;
        }

        spAnimationStateData_setMix( m_animationStateData, animation_first, animation_second, _mix );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_spAnimationStateListener( spAnimationState * _state, spEventType _type, spTrackEntry * _entry, spEvent * _event )
        {
            Spine * spine = static_cast<Spine *>(_state->rendererObject);

            spine->addAnimationEvent_( _state, _type, _entry, _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimation( const ConstString & _state, const ConstString & _name, float _timing, float _speedFactor, bool _loop )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( this->isCompile() == true )
        {
            if( it_found != m_animations.end() )
            {
                const AnimationDesc & desc = *it_found;

                spAnimationState * animationState = desc.animationState;

                spAnimationState_clearTracks( animationState );
                spAnimationState_dispose( animationState );

                m_animations.erase( it_found );
            }

            spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( _name );

            MENGINE_ASSERTION_MEMORY_PANIC( animation, false, "'%s' invalid found animation '%s'"
                , this->getName().c_str()
                , _name.c_str()
            );

            spAnimationState * animationState = spAnimationState_create( m_animationStateData );

            animationState->rendererObject = this;
            animationState->listener = &Detail::s_spAnimationStateListener;

            spTrackEntry * track = spAnimationState_setAnimation( animationState, 0, animation, _loop ? 1 : 0 );

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = animationState;
            desc.track = track;
            desc.time = _timing;
            desc.duration = animation->duration * 1000.f;
            desc.speedFactor = _speedFactor;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = _loop;

            float spTiming = desc.time * 0.001f;
            spAnimationState_update( animationState, spTiming );
            spAnimationState_apply( animationState, m_skeleton );

            m_animations.emplace_back( desc );
        }
        else
        {
            if( it_found != m_animations.end() )
            {
                m_animations.erase( it_found );
            }

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = nullptr;
            desc.time = _timing;
            desc.duration = -1.f;
            desc.speedFactor = _speedFactor;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = _loop;

            m_animations.emplace_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::removeStateAnimation( const ConstString & _state )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        if( this->isCompile() == true )
        {
            const AnimationDesc & desc = *it_found;

            spAnimationState * animationState = desc.animationState;

            spAnimationState_clearTracks( animationState );
            spAnimationState_dispose( animationState );
        }

        m_animations.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::hasSkeletonAnimationName( const ConstString & _name ) const
    {
        spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( _name );

        if( animation == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        AnimationDesc & desc = *it_found;

        desc.speedFactor = _speedFactor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationSpeedFactor( const ConstString & _state ) const
    {
        VectorAnimations::const_iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        const AnimationDesc & desc = *it_found;

        return desc.speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationTiming( const ConstString & _state, float _timing )
    {
        VectorAnimations::const_iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        const AnimationDesc & desc = *it_found;

        if( this->setStateAnimation( _state, desc.name, _timing, desc.speedFactor, desc.loop ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationFreeze( const ConstString & _state, bool _freeze )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        AnimationDesc & desc = *it_found;

        desc.freeze = _freeze;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::getStateAnimationFreeze( const ConstString & _state ) const
    {
        VectorAnimations::const_iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        const AnimationDesc & desc = *it_found;

        return desc.freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationTiming( const ConstString & _state ) const
    {
        VectorAnimations::const_iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        const AnimationDesc & desc = *it_found;

        return desc.time;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationDuration( const ConstString & _state ) const
    {
        VectorAnimations::const_iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( "'%s' invalid found state animation '%s'"
                , this->getName().c_str()
                , _state.c_str()
            );

            return false;
        }

        const AnimationDesc & desc = *it_found;

        return desc.duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationFirstFrame( const ConstString & _state, const ConstString & _name )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( this->isCompile() == true )
        {
            if( it_found != m_animations.end() )
            {
                const AnimationDesc & desc = *it_found;

                spAnimationState * animationState = desc.animationState;

                spAnimationState_clearTracks( animationState );
                spAnimationState_dispose( animationState );

                m_animations.erase( it_found );
            }

            spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( _name );

            MENGINE_ASSERTION_MEMORY_PANIC( animation, false, "'%s' invalid found animation '%s'"
                , this->getName().c_str()
                , _name.c_str()
            );

            spAnimationState * animationState = spAnimationState_create( m_animationStateData );

            animationState->rendererObject = this;
            animationState->listener = &Detail::s_spAnimationStateListener;

            spTrackEntry * track = spAnimationState_setAnimation( animationState, 0, animation, 0 );

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = animationState;
            desc.track = track;
            desc.time = 0.f;
            desc.duration = animation->duration * 1000.f;
            desc.speedFactor = 1.f;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = false;

            float spTiming = desc.time * 0.001f;
            spAnimationState_update( animationState, spTiming );
            spAnimationState_apply( animationState, m_skeleton );

            m_animations.emplace_back( desc );
        }
        else
        {
            if( it_found != m_animations.end() )
            {
                m_animations.erase( it_found );
            }

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = nullptr;
            desc.time = 0.f;
            desc.duration = -1.f;
            desc.speedFactor = 1.f;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = false;

            m_animations.emplace_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationLastFrame( const ConstString & _state, const ConstString & _name )
    {
        VectorAnimations::iterator it_found = std::find_if( m_animations.begin(), m_animations.end(), [&_state]( const AnimationDesc & _desc )
        {
            return _desc.state == _state;
        } );

        if( this->isCompile() == true )
        {
            if( it_found != m_animations.end() )
            {
                const AnimationDesc & desc = *it_found;

                spAnimationState * animationState = desc.animationState;

                spAnimationState_clearTracks( animationState );
                spAnimationState_dispose( animationState );

                m_animations.erase( it_found );
            }

            spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( _name );

            MENGINE_ASSERTION_MEMORY_PANIC( animation, false, "'%s' invalid found animation '%s'"
                , this->getName().c_str()
                , _name.c_str()
            );

            spAnimationState * animationState = spAnimationState_create( m_animationStateData );

            animationState->rendererObject = this;
            animationState->listener = &Detail::s_spAnimationStateListener;

            spTrackEntry * track = spAnimationState_setAnimation( animationState, 0, animation, 0 );

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = animationState;
            desc.track = track;
            desc.time = animation->duration * 1000.f;
            desc.duration = animation->duration * 1000.f;
            desc.speedFactor = 1.f;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = false;

            float spTiming = desc.time * 0.001f;
            spAnimationState_update( animationState, spTiming );
            spAnimationState_apply( animationState, m_skeleton );

            spSkeleton_updateCache( m_skeleton );
            spSkeleton_updateWorldTransform( m_skeleton );

            m_animations.emplace_back( desc );
        }
        else
        {
            if( it_found != m_animations.end() )
            {
                m_animations.erase( it_found );
            }

            AnimationDesc desc;

            desc.state = _state;
            desc.name = _name;
            desc.animationState = nullptr;
            desc.time = -1.f;
            desc.duration = -1.f;
            desc.speedFactor = 1.f;
            desc.freeze = false;
            desc.complete = false;
            desc.loop = false;

            m_animations.emplace_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceSpineSkeleton, false, "'%s' resource is null"
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

        MENGINE_ASSERTION_MEMORY_PANIC( animationStateData, false );

        m_animationStateData = animationStateData;

        spSkeleton * skeleton = spSkeleton_create( skeletonData );

        MENGINE_ASSERTION_MEMORY_PANIC( skeleton, false );

        m_skeleton = skeleton;

        spSkeleton_setToSetupPose( m_skeleton );

        for( AnimationDesc & desc : m_animations )
        {
            spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( desc.name );

            MENGINE_ASSERTION_MEMORY_PANIC( animation, false, "'%s' invalid found animation '%s'"
                , this->getName().c_str()
                , desc.name.c_str()
            );

            spAnimationState * animationState = spAnimationState_create( m_animationStateData );

            animationState->rendererObject = this;
            animationState->listener = &Detail::s_spAnimationStateListener;

            spAnimationState_setAnimation( animationState, 0, animation, desc.loop ? 1 : 0 );

            desc.animationState = animationState;
            desc.duration = animation->duration * 1000.f;

            if( desc.time < 0.f )
            {
                desc.time = desc.duration;
            }

            float spTiming = desc.time * 0.001f;

            spAnimationState_update( animationState, spTiming );
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

        for( AnimationDesc & desc : m_animations )
        {
            spAnimationState * animationState = desc.animationState;

            spAnimationState_clearTracks( animationState );
            spAnimationState_dispose( animationState );

            desc.animationState = nullptr;
            desc.duration = -1.f;
        }

        m_animations.clear();

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

        m_events.clear();
        m_eventsAux.clear();
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

        MENGINE_ASSERTION_MEMORY_PANIC( material, nullptr, "'%s' resource '%s' image '%s' m_material is NULL"
            , this->getName().c_str()
            , m_resourceSpineSkeleton->getName().c_str()
            , _resourceImage->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::addAnimationEvent_( spAnimationState * _animationState, spEventType _type, spTrackEntry * _entry, spEvent * _event )
    {
        MENGINE_UNUSED( _event );

        AnimationEventDesc ev;

        ev.trackIndex = _entry->trackIndex;
        ev.type = _type;

        bool event_valid = false;

        switch( _type )
        {
        case SP_ANIMATION_START:
        case SP_ANIMATION_END:
        case SP_ANIMATION_COMPLETE:
            {
                ev.eventName = nullptr;
                ev.eventIntValue = 0;
                ev.eventFloatValue = 0.f;
                ev.eventStringValue = nullptr;

                for( AnimationDesc & desc : m_animations )
                {
                    const ConstString & state = desc.state;

                    if( desc.animationState != _animationState )
                    {
                        continue;
                    }

                    if( desc.loop == true )
                    {
                        break;
                    }

                    desc.complete = true;
                    desc.time = desc.duration;

                    ev.state = state;
                    ev.name = desc.name;

                    event_valid = true;
                    break;
                }
            }break;
        case SP_ANIMATION_EVENT:
            {
                ev.eventName = _event->data->name;
                ev.eventIntValue = _event->intValue;
                ev.eventFloatValue = _event->floatValue;
                ev.eventStringValue = _event->stringValue != nullptr ? _event->stringValue : "";

                event_valid = true;
            }break;
        }

        ev.loopCount = _entry->loop;

        if( event_valid == false )
        {
            return;
        }

        m_events.push_back( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        if( m_animations.empty() == true )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        float spTiming = totalTime * 0.001f;

        spSkeleton_update( m_skeleton, spTiming );

        for( AnimationDesc & desc : m_animations )
        {
            if( desc.freeze == true )
            {
                continue;
            }

            if( desc.complete == true )
            {
                continue;
            }

            float an_time = totalTime * desc.speedFactor;

            if( desc.duration != 0.f )
            {
                desc.time += an_time;

                while( desc.time >= desc.duration )
                {
                    desc.time -= desc.duration;
                }
            }
            else
            {
                desc.time = 0.f;
            }

            spAnimationState * animationState = desc.animationState;

            float sp_an_timing = an_time * 0.001f;

            spAnimationState_update( animationState, sp_an_timing );
            spAnimationState_apply( animationState, m_skeleton );
        }

        spSkeleton_updateWorldTransform( m_skeleton );

        m_eventsAux = std::move( m_events );

        bool loop = this->isLoop();

        for( const AnimationEventDesc & desc : m_eventsAux )
        {
            switch( desc.type )
            {
            case SP_ANIMATION_COMPLETE:
                {
                    EVENTABLE_METHOD( EVENT_SPINE_STATE_ANIMATION_END )
                        ->onSpineStateAnimationEnd( desc.state, desc.name, true );

                    if( loop == false )
                    {
                        this->end();
                    }
                }break;
            case SP_ANIMATION_EVENT:
                {
                    EVENTABLE_METHOD( EVENT_SPINE_EVENT )
                        ->onSpineEvent( desc.eventName, desc.eventIntValue, desc.eventFloatValue, desc.eventStringValue );
                }break;
            }
        }
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

        bool autoPlay = this->isAutoPlay();

        if( autoPlay == true )
        {
            float time = TIMELINE_SERVICE()
                ->getTotalTime();

            if( this->play( time ) == 0 )
            {
                LOGGER_ERROR( "'%s' resource '%s' auto play return 0"
                    , this->getName().c_str()
                    , this->m_resourceSpineSkeleton->getName().c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_play( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );

        if( this->isActivate() == false )
        {
            LOGGER_ERROR( "'%s' play not activate"
                , this->getName().c_str()
            );

            return false;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_restart( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_pause( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_resume( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_stop( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_interrupt( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _enumerator );

        return true;
    }
}