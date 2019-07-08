#	include "Spine.h"

#	include "Kernel/Materialable.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	ifndef MENGINE_SPINE_MAX_VERTICES
#	define MENGINE_SPINE_MAX_VERTICES 512
#	endif

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
    void Spine::setResourceSpine( const ResourceSpinePtr & _resourceSpine )
    {
        if( m_resourceSpine == _resourceSpine )
        {
            return;
        }

        m_resourceSpine = _resourceSpine;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceSpinePtr & Spine::getResourceSpine() const
    {
        return m_resourceSpine;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::mixAnimation( const ConstString & _first, const ConstString & _second, float _mix )
    {
        if( m_resourceSpine == nullptr )
        {
            return false;
        }

        spAnimation * animation_first = m_resourceSpine->findSkeletonAnimation( _first );

        if( animation_first == nullptr )
        {
            return false;
        }

        spAnimation * animation_second = m_resourceSpine->findSkeletonAnimation( _second );

        if( animation_second == nullptr )
        {
            return false;
        }

        spAnimationStateData_setMix( m_animationStateData, animation_first, animation_second, _mix );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_spAnimationStateListener( spAnimationState * state, int trackIndex, spEventType type, spEvent * event, int loopCount )
    {
        Spine * spine = static_cast<Spine *>(state->rendererObject);

        spine->addAnimationEvent( state, trackIndex, type, event, loopCount );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimation( const ConstString & _state, const ConstString & _name, float _timing, float _speedFactor, bool _loop )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::setStateAnimation %s invalid setup state '%s' name '%s' not compile!"
                , this->getName().c_str()
                , _state.c_str()
                , _name.c_str()
                );

            return false;
        }

        TMapAnimations::iterator it_found = m_animations.find( _state );

        if( it_found != m_animations.end() )
        {
            spAnimationState * state = it_found->second.state;

            spAnimationState_clearTracks( state );

            spAnimationState_dispose( state );

            m_animations.erase( it_found );
        }

        spAnimation * animation = m_resourceSpine->findSkeletonAnimation( _name );

        if( animation == nullptr )
        {
            return false;
        }

        spAnimationState * state = spAnimationState_create( m_animationStateData );

        state->rendererObject = this;
        state->listener = &s_spAnimationStateListener;

        spAnimationState_setAnimation( state, 0, animation, _loop ? 1 : 0 );

        Animation an;

        an.name = _name;
        an.state = state;
        an.timing = _timing;
        an.duration = animation->duration * 1000.f;
        an.speedFactor = _speedFactor;
        an.freeze = false;
        an.complete = false;
        an.loop = _loop;

        float spTiming = an.timing * 0.001f;

        spAnimationState_update( state, spTiming );

        m_animations.insert( std::make_pair( _state, an ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::removeStateAnimation( const ConstString & _state )
    {
        TMapAnimations::iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::removeAnimation invalid not found state '%s'"
                , _state.c_str()
                );

            return false;
        }

        Animation & an = it_found->second;

        spAnimationState * state = an.state;

        spAnimationState_clearTracks( state );

        spAnimationState_dispose( state );

        m_animations.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor )
    {
        TMapAnimations::iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::setAnimationSpeedFactor invalid not found state '%s'"
                , _state.c_str()
                );

            return false;
        }

        Animation & an = it_found->second;

        an.speedFactor = _speedFactor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationSpeedFactor( const ConstString & _state ) const
    {
        TMapAnimations::const_iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getAnimationSpeedFactor invalid not found state '%s'"
                , _state.c_str()
                );

            return 0.f;
        }

        const Animation & an = it_found->second;

        return an.speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationTiming( const ConstString & _state, float _timing )
    {
        TMapAnimations::const_iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::setAnimationTiming invalid not found state '%s'"
                , _state.c_str()
                );

            return false;
        }

        Animation an = it_found->second;

        if( this->setStateAnimation( _state, an.name, _timing, an.speedFactor, an.loop ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::setStateAnimationFreeze( const ConstString & _state, bool _freeze )
    {
        TMapAnimations::iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::setAnimationFreeze invalid not found state '%s'"
                , _state.c_str()
                );

            return false;
        }

        Animation & an = it_found->second;

        an.freeze = _freeze;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::getStateAnimationFreeze( const ConstString & _state ) const
    {
        TMapAnimations::const_iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getAnimationFreeze invalid not found state '%s'"
                , _state.c_str()
                );

            return false;
        }

        const Animation & an = it_found->second;

        return an.freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationTiming( const ConstString & _state ) const
    {
        TMapAnimations::const_iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getAnimationSpeedFactor invalid not found state '%s'"
                , _state.c_str()
                );

            return 0.f;
        }

        const Animation & an = it_found->second;

        return an.timing;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getStateAnimationDuration( const ConstString & _state ) const
    {
        TMapAnimations::const_iterator it_found = m_animations.find( _state );

        if( it_found == m_animations.end() )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getStateAnimationDuration invalid not found state '%s'"
                , _state.c_str()
                );

            return 0.f;
        }

        const Animation & an = it_found->second;

        return an.duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float Spine::getAnimationDuration( const ConstString & _name )
    {
        if( m_resourceSpine == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getDuration %s not setup resource"
                , this->getName().c_str()
                );

            return 0.f;
        }

        //TODO FIXME
        bool compile = m_resourceSpine.isCompile();

        if( compile == false )
        {
            m_resourceSpine.compile();
        }

        spAnimation * animation = m_resourceSpine->findSkeletonAnimation( _name );

        if( animation == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::getDuration %s invalid find skeleton animation %s"
                , this->getName().c_str()
                , _name.c_str()
                );

            return 0.f;
        }

        float duration = animation->duration * 1000.f;

        if( compile == false )
        {
            m_resourceSpine.release();
        }

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    //void Spine::_setEventListener( const pybind::dict & _listener )
    //{		
        //this->registerEvent( EVENT_SPINE_END, ("onSpineEnd"), _listener );
        //this->registerEvent( EVENT_SPINE_STOP, ("onSpineStop"), _listener );
        //this->registerEvent( EVENT_SPINE_PAUSE, ("onSpinePause"), _listener );
        //this->registerEvent( EVENT_SPINE_RESUME, ("onSpineResume"), _listener );
        //this->registerEvent( EVENT_SPINE_EVENT, ("onSpineEvent"), _listener );
        //this->registerEvent( EVENT_SPINE_STATE_ANIMATION_END, ("onSpineStateAnimationEnd"), _listener );

        //this->registerEvent( EVENT_ANIMATABLE_END, ("onAnimatableEnd"), _listener );
    //}
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_compile()
    {
        if( m_resourceSpine == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::_compile: '%s' resource is null"
                , m_name.c_str()
                );

            return false;
        }

        if( m_resourceSpine.compile() == false )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::_compile: '%s' resource '%s' is not compile"
                , m_name.c_str()
                , m_resourceSpine->getName().c_str()
                );

            return false;
        }

        spSkeletonData * skeletonData = m_resourceSpine->getSkeletonData();

        spSkeleton * skeleton = spSkeleton_create( skeletonData );

        spAnimationStateData * animationStateData = spAnimationStateData_create( skeletonData );

        //spSkeleton_update( skeleton, 0.f );

        //spSkeleton_updateWorldTransform( skeleton );

        m_skeleton = skeleton;
        m_animationStateData = animationStateData;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_release()
    {
        Node::_release();

        for( TMapAnimations::iterator
            it = m_animations.begin(),
            it_end = m_animations.end();
            it != it_end;
            ++it )
        {
            spAnimationState * state = it->second.state;

            spAnimationState_clearTracks( state );

            spAnimationState_dispose( state );
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

        m_resourceSpine.release();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Spine::makeMaterial_( spSlot * _slot, ResourceImage * _resourceImage ) const
    {
        EMaterialBlendMode blendMode = EMB_NORMAL;

        switch( _slot->data->blendMode )
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

        RenderMaterialInterfacePtr material = Helper::makeImageMaterial( m_serviceProvider, _resourceImage, ConstString::none(), blendMode, false, false );

        if( material == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::updateMaterial_ %s resource %s image %s m_material is NULL"
                , this->getName().c_str()
                , m_resourceSpine->getName().c_str()
                , _resourceImage->getName().c_str()
                );

            return nullptr;
        }

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::addAnimationEvent( spAnimationState * _state, int _trackIndex, spEventType _type, spEvent * _event, int _loopCount )
    {
        (void)_loopCount;
        (void)_event;

        AnimationEvent ev;

        ev.trackIndex = _trackIndex;
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

                for( TMapAnimations::iterator
                    it = m_animations.begin(),
                    it_end = m_animations.end();
                    it != it_end;
                    ++it )
                {
                    const ConstString & key = it->first;

                    Animation & an = it->second;

                    if( an.state != _state )
                    {
                        continue;
                    }

                    if( an.loop == true )
                    {
                        break;
                    }

                    an.complete = true;
                    an.timing = an.duration;

                    ev.state = key;
                    ev.animation = an.name;

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

        ev.loopCount = _loopCount;

        if( event_valid == false )
        {
            return;
        }

        m_events.push_back( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_update( float _current, float _timing )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        if( m_animations.empty() == true )
        {
            return;
        }

        //if( this->getResourceSpine()->getName() == "Effect_BuyUpgrade" )
        //{
        //	printf( "!" );
        //}

        if( m_playTime > _current )
        {
            float deltha = m_playTime - _current;
            _timing -= deltha;
        }

        float speedFactor = this->getAnimationSpeedFactor();
        float scretch = this->getStretch();
        float total_timing = _timing * speedFactor / scretch;

        float spTiming = total_timing * 0.001f;

        spSkeleton_update( m_skeleton, spTiming );

        for( TMapAnimations::iterator
            it = m_animations.begin(),
            it_end = m_animations.end();
            it != it_end;
            ++it )
        {
            Animation & an = it->second;

            if( an.freeze == true )
            {
                continue;
            }

            if( an.complete == true )
            {
                continue;
            }

            float an_timing = total_timing * an.speedFactor;

            an.timing += an_timing;

            while( an.timing >= an.duration )
            {
                an.timing -= an.duration;
            }

            spAnimationState * state = an.state;

            float sp_an_timing = an_timing * 0.001f;

            spAnimationState_update( state, sp_an_timing );
            spAnimationState_apply( state, m_skeleton );
        }

        spSkeleton_updateWorldTransform( m_skeleton );

        TVectorAnimationEvent events;
        m_events.swap( events );

        bool loop = this->getLoop();

        for( TVectorAnimationEvent::const_iterator
            it = events.begin(),
            it_end = events.end();
            it != it_end;
            ++it )
        {
            const AnimationEvent & ev = *it;

            switch( ev.type )
            {
            case SP_ANIMATION_COMPLETE:
                {
                    EVENTABLE_METHOD( this, EVENT_SPINE_STATE_ANIMATION_END )
                        ->onSpineStateAnimationEnd( ev.state, ev.animation, true );
                    //EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_STATE_ANIMATION_END )(this, ev.state, ev.animation, true);

                    if( loop == false )
                    {
                        this->end();
                    }
                }break;
            case SP_ANIMATION_EVENT:
                {
                    EVENTABLE_METHOD( this, EVENT_SPINE_EVENT )
                        ->onSpineEvent( ev.eventName, ev.eventIntValue, ev.eventFloatValue, ev.eventStringValue );
                    //EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_EVENT )(this, ev.eventName, ev.eventIntValue, ev.eventFloatValue, ev.eventStringValue);
                }break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_render( RenderServiceInterface * _renderService, const RenderObjectState * _state )
    {
        //if( this->getResourceSpine()->getName() == "Effect_BuyUpgrade" )
        //{
        //	printf( "!" );
        //}

        const mt::box2f & bb = this->getBoundingBox();

        const mt::mat4f & wm = this->getWorldMatrix();

        int slotCount = m_skeleton->slotsCount;

        const int quadTriangles[6] = {0, 1, 2, 2, 3, 0};

        float attachment_vertices[MENGINE_SPINE_MAX_VERTICES * 2];

        ColourValue color;
        this->calcTotalColor( color );

        float nr = color.getR();
        float ng = color.getG();
        float nb = color.getB();
        float na = color.getA();

        for( int i = 0; i != slotCount; ++i )
        {
            spSlot * slot = m_skeleton->drawOrder[i];

            if( slot->attachment == nullptr )
            {
                continue;
            }

            AttachmentMesh & mesh = m_attachmentMeshes[slot->data->name];

            const float * uvs = nullptr;
            int verticesCount;
            const int * triangles;
            int trianglesCount;

            float ar = 1.f;
            float ag = 1.f;
            float ab = 1.f;
            float aa = 1.f;

            const spAttachmentType attachment_type = slot->attachment->type;

            ResourceImage * resourceImage = nullptr;

            switch( attachment_type )
            {
            case SP_ATTACHMENT_REGION:
                {
                    spRegionAttachment * attachment = (spRegionAttachment *)slot->attachment;

                    spRegionAttachment_computeWorldVertices( attachment, slot->bone, attachment_vertices );

                    uvs = attachment->uvs;
                    verticesCount = 8;
                    triangles = quadTriangles;
                    trianglesCount = 6;
                    ar = attachment->r;
                    ag = attachment->g;
                    ab = attachment->b;
                    aa = attachment->a;

                    resourceImage = (ResourceImage *)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

                    mesh.vertices.resize( 4 );
                    mesh.indices.resize( 6 );
                }break;
            case SP_ATTACHMENT_MESH:
                {
                    spMeshAttachment * attachment = (spMeshAttachment *)slot->attachment;

                    spMeshAttachment_computeWorldVertices( attachment, slot, attachment_vertices );

                    uvs = attachment->uvs;
                    verticesCount = attachment->verticesCount;
                    triangles = attachment->triangles;
                    trianglesCount = attachment->trianglesCount;
                    ar = attachment->r;
                    ag = attachment->g;
                    ab = attachment->b;
                    aa = attachment->a;

                    resourceImage = (ResourceImage *)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

                    mesh.vertices.resize( attachment->verticesCount / 2 );
                    mesh.indices.resize( attachment->trianglesCount );
                }break;
            case SP_ATTACHMENT_SKINNED_MESH:
                {
                    spSkinnedMeshAttachment * attachment = (spSkinnedMeshAttachment *)slot->attachment;

                    spSkinnedMeshAttachment_computeWorldVertices( attachment, slot, attachment_vertices );

                    uvs = attachment->uvs;
                    verticesCount = attachment->uvsCount;
                    triangles = attachment->triangles;
                    trianglesCount = attachment->trianglesCount;
                    ar = attachment->r;
                    ag = attachment->g;
                    ab = attachment->b;
                    aa = attachment->a;

                    resourceImage = (ResourceImage *)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

                    mesh.vertices.resize( attachment->uvsCount / 2 );
                    mesh.indices.resize( attachment->trianglesCount );
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

            float wr = nr * m_skeleton->r * slot->r * ar;
            float wg = ng * m_skeleton->g * slot->g * ag;
            float wb = nb * m_skeleton->b * slot->b * ab;
            float wa = na * m_skeleton->a * slot->a * aa;

            ColourValue_ARGB argb = Helper::makeARGB( wr, wg, wb, wa );

            RenderVertex2D * vertices = mesh.vertices.buff();
            RenderIndices * indices = mesh.indices.buff();

            this->fillVertices_( vertices, attachment_vertices, uvs, argb, verticesCount / 2, wm );
            this->fillIndices_( indices, triangles, trianglesCount );

            const RenderMaterialInterfacePtr & material = mesh.material;

            _renderService
                ->addRenderObject( _state, material, vertices, verticesCount / 2, indices, trianglesCount, &bb, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColourValue_ARGB _argb, int _count, const mt::mat4f & _wm )
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
    void Spine::fillIndices_( RenderIndices * _indices, const int * _triangles, int _count )
    {
        for( int i = 0; i != _count; ++i )
        {
            _indices[i] = (RenderIndices)_triangles[i];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_play( float _time )
    {
        (void)_time;

        if( this->isActivate() == false )
        {
            LOGGER_ERROR( m_serviceProvider )("Spine::_play: '%s' play not activate"
                , this->getName().c_str()
                );

            return false;
        }

        //bool loop = this->getLoop();

        //spAnimationState_setAnimation( m_animationState, 0, m_currentAnimation, loop ? 1 : 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_restart( uint32_t _enumerator, float _time )
    {
        (void)_time;
        (void)_enumerator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_pause( uint32_t _enumerator )
    {
        (void)_enumerator;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_PAUSE )
            ->onAnimatablePause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_resume( uint32_t _enumerator, float _time )
    {
        (void)_enumerator;
        (void)_time;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_RESUME )
            ->onAnimatableResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_stop( uint32_t _enumerator )
    {
        (void)_enumerator;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_STOP )
            ->onAnimatableStop( _enumerator );
        //EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_END )(this, _enumerator, false);
        //EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
    }
    //////////////////////////////////////////////////////////////////////////
    void Spine::_end( uint32_t _enumerator )
    {
        (void)_enumerator;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
        //EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_END )(this, _enumerator, true);
        //EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
    }
    //////////////////////////////////////////////////////////////////////////
    bool Spine::_interrupt( uint32_t _enumerator )
    {
        (void)_enumerator;

        return true;
    }
}