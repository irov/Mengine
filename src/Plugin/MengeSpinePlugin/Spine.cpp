#	include "Spine.h"

#	include "Kernel/Materialable.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	ifndef MENGINE_SPINE_MAX_VERTICES
#	define MENGINE_SPINE_MAX_VERTICES 512
#	endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Spine::Spine()
		: m_skeleton( nullptr )
		, m_animationStateData( nullptr )
		, m_animationState( nullptr )
		, m_currentAnimation( nullptr )
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
	void Spine::setAnimationName( const ConstString & _name )
	{
		if( m_currentAnimationName == _name )
		{
			return;
		}

		m_currentAnimationName = _name;

		if( this->isCompile() == false )
		{
			return;
		}

		this->updateAnimation_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::updateAnimation_()
	{
		if( m_currentAnimationName.empty() == true )
		{
			m_currentAnimation = nullptr;

			return;
		}
		
		spAnimation * animation = m_resourceSpine->findSkeletonAnimation( m_currentAnimationName );

		if( animation == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Spine::updateAnimation_ %s invalid find skeleton animation %s"
				, this->getName().c_str()
				, m_currentAnimationName.c_str()
				);

			return;
		}

		m_currentAnimation = animation;

		bool loop = this->getLoop();

		spAnimationState_setAnimation( m_animationState, 0, m_currentAnimation, loop ? 1 : 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::setAnimationMix( const ConstString & _from, const ConstString & _to, float _time )
	{
		spAnimationStateData_setMixByName( m_animationStateData, _from.c_str(), _to.c_str(), _time );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Spine::getAnimationName() const
	{
		return m_currentAnimationName;
	}
	//////////////////////////////////////////////////////////////////////////
	float Spine::getDuration( const ConstString & _name )
	{
		if( m_resourceSpine == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Spine::getDuration %s not setup resource"
				, this->getName().c_str()
				, m_currentAnimationName.c_str()
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
				, m_currentAnimationName.c_str()
				);

			return 0.f;
		}

		float duration = animation->duration;

		if( compile == false )
		{
			m_resourceSpine.release();
		}		

		return duration;
	}
	//////////////////////////////////////////////////////////////////////////
	enum SpineEventFlag
	{
		EVENT_SPINE_END = 0,
		EVENT_SPINE_STOP,
		EVENT_SPINE_PAUSE,
		EVENT_SPINE_RESUME,
		EVENT_SPINE_EVENT,

		EVENT_ANIMATABLE_END
	};
	//////////////////////////////////////////////////////////////////////////
	void Spine::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_SPINE_END, ("onSpineEnd"), _listener );
		this->registerEvent( EVENT_SPINE_STOP, ("onSpineStop"), _listener );
		this->registerEvent( EVENT_SPINE_PAUSE, ("onSpinePause"), _listener );
		this->registerEvent( EVENT_SPINE_RESUME, ("onSpineResume"), _listener );
		this->registerEvent( EVENT_SPINE_EVENT, ("onSpineEvent"), _listener );

		this->registerEvent( EVENT_ANIMATABLE_END, ("onAnimatableEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_spAnimationStateListener( spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount )
	{
		Spine * spine = static_cast<Spine*>(state->rendererObject);

		spine->addAnimationEvent( trackIndex, type, event, loopCount );
	}
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
		
		spAnimationState * animationState = spAnimationState_create( animationStateData );

		animationState->rendererObject = this;
		animationState->listener = &s_spAnimationStateListener;

		spSkeleton_update( skeleton, 0.f );
		spAnimationState_update( animationState, 0.f );
		spAnimationState_apply( animationState, skeleton );

		spSkeleton_updateWorldTransform( skeleton );

		int slotCount = skeleton->slotsCount;

		m_attachmentMeshes.resize( slotCount );

		for( int i = 0; i < slotCount; ++i )
		{
			spSlot * slot = skeleton->slots[i];

			if( slot->attachment == nullptr )
			{
				continue;
			}

			AttachmentMesh & mesh = m_attachmentMeshes[i];

			ResourceImage * resourceImage = nullptr;

			const spAttachmentType attachment_type = slot->attachment->type;

			switch( attachment_type )
			{
			case SP_ATTACHMENT_REGION:
				{
					spRegionAttachment * attachment = (spRegionAttachment *)slot->attachment;

					resourceImage = (ResourceImage*)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);
								
					mesh.vertices.resize( 4 );
					mesh.indices.resize( 6 );
				}break;
			case SP_ATTACHMENT_MESH:
				{
					spMeshAttachment * attachment = (spMeshAttachment *)slot->attachment;

					resourceImage = (ResourceImage*)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

					mesh.vertices.resize( attachment->verticesCount / 2 );
					mesh.indices.resize( attachment->trianglesCount );
				}break;
			case SP_ATTACHMENT_SKINNED_MESH:
				{
					spSkinnedMeshAttachment* attachment = (spSkinnedMeshAttachment*)slot->attachment;

					resourceImage = (ResourceImage*)(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject);

					mesh.vertices.resize( attachment->uvsCount / 2 );
					mesh.indices.resize( attachment->trianglesCount );
				}break;
			default:
				continue;
				break;
			}

			if( resourceImage == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Spine::_compile: '%s' resource '%s' image not setup for attachment '%d' type '%d'"
					, m_name.c_str()
					, m_resourceSpine->getName().c_str()
					, i
					, attachment_type
					);

				return false;
			}

			mesh.material = this->makeMaterial_( slot, resourceImage );
		}

		m_skeleton = skeleton;
		m_animationStateData = animationStateData;
		m_animationState = animationState;

		this->updateAnimation_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_release()
	{
		Node::_release();

		m_resourceSpine.release();

		if( m_skeleton != nullptr )
		{
			spSkeleton_dispose( m_skeleton );
			m_skeleton = nullptr;
		}

		if( m_animationState != nullptr )
		{
			spAnimationState_dispose( m_animationState );
			m_animationState = nullptr;
		}

		if( m_animationStateData != nullptr )
		{
			spAnimationStateData_dispose( m_animationStateData );
			m_animationStateData = nullptr;
		}

		m_attachmentMeshes.clear();

		m_currentAnimation = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Spine::makeMaterial_( spSlot * slot, ResourceImage * _resourceImage ) const
	{ 
		EMaterialBlendMode blendMode = EMB_NORMAL;

		switch( slot->data->blendMode )
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
	void Spine::addAnimationEvent( int _trackIndex, spEventType _type, spEvent * _event, int _loopCount )
	{
		(void)_loopCount;
		(void)_event;

		AnimationEvent ev;

		ev.trackIndex = _trackIndex;
		ev.type = _type;

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
			}break;
		case SP_ANIMATION_EVENT:
			{
				ev.eventName = _event->data->name;
				ev.eventIntValue = _event->intValue;
				ev.eventFloatValue = _event->floatValue;
				ev.eventStringValue = _event->stringValue != nullptr ? _event->stringValue : "";
			}break;
		}

		ev.loopCount = _loopCount;
		
		m_events.push_back( ev );
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_update( float _current, float _timing )
	{
		if( m_currentAnimation == nullptr )
		{
			return;
		}

		if( this->isPlay() == false )
		{ 
			return;
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

		float spTiming = _timing * 0.001f;

		spSkeleton_update( m_skeleton, spTiming );
		spAnimationState_update( m_animationState, spTiming );

		spAnimationState_apply( m_animationState, m_skeleton );

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
					if( loop == false )
					{
						this->end();
					}
				}break;
			case SP_ANIMATION_EVENT:
				{
					EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_EVENT )(this, ev.eventName, ev.eventIntValue, ev.eventFloatValue, ev.eventStringValue);
				}break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_render( const RenderObjectState * _state )
	{
		if( m_currentAnimation == nullptr )
		{
			return;
		}

		const mt::box2f & bb = this->getBoundingBox();

		const mt::mat4f & wm = this->getWorldMatrix();

		int slotCount = m_skeleton->slotsCount;

		const int quadTriangles[6] = {0, 1, 2, 2, 3, 0};

		float attachment_vertices[MENGINE_SPINE_MAX_VERTICES * 2];

		const float * uvs = nullptr;
		int verticesCount;
		const int * triangles;
		int trianglesCount;
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 0.f;

		for( int i = 0; i < slotCount; ++i )
		{
			spSlot * slot = m_skeleton->slots[i];
			
			if( slot->attachment == nullptr )
			{
				continue;
			}

			const spAttachmentType attachment_type = slot->attachment->type;

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
					r = attachment->r;
					g = attachment->g;
					b = attachment->b;
					a = attachment->a;
				}break;
			case SP_ATTACHMENT_MESH:
				{
					spMeshAttachment * attachment = (spMeshAttachment *)slot->attachment;

					spMeshAttachment_computeWorldVertices( attachment, slot, attachment_vertices );

					uvs = attachment->uvs;
					verticesCount = attachment->verticesCount;
					triangles = attachment->triangles;
					trianglesCount = attachment->trianglesCount;
					r = attachment->r;
					g = attachment->g;
					b = attachment->b;
					a = attachment->a;
				}break;
			case SP_ATTACHMENT_SKINNED_MESH:
				{
					spSkinnedMeshAttachment* attachment = (spSkinnedMeshAttachment*)slot->attachment;
					
					spSkinnedMeshAttachment_computeWorldVertices( attachment, slot, attachment_vertices );
					
					uvs = attachment->uvs;
					verticesCount = attachment->uvsCount;
					triangles = attachment->triangles;
					trianglesCount = attachment->trianglesCount;
					r = attachment->r;
					g = attachment->g;
					b = attachment->b;
					a = attachment->a;
				}break;
			default:
				continue;
				break;
			}

			float wr = m_skeleton->r * slot->r * r;
			float wg = m_skeleton->g * slot->g * g;
			float wb = m_skeleton->b * slot->b * b;
			float wa = m_skeleton->a * slot->a * a;

			ColourValue_ARGB argb = Helper::makeARGB( wr, wg, wb, wa );

			RenderVertex2D * vertices = m_attachmentMeshes[i].vertices.buff();
			RenderIndices * indices = m_attachmentMeshes[i].indices.buff();
				
			this->fillVertices_( vertices, attachment_vertices, uvs, argb, verticesCount / 2, wm );
			this->fillIndices_( indices, triangles, trianglesCount );

			const RenderMaterialInterfacePtr & material = m_attachmentMeshes[i].material;

			RENDER_SERVICE( m_serviceProvider )
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

			mt::vec3f v;
			v.x = _vertices[index_x];
			v.y = -_vertices[index_y];
			v.z = 0.f;

			RenderVertex2D & vertex = _vertices2D[i];

			mt::mul_v3_m4( vertex.pos, v, _wm );

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
				
		bool loop = this->getLoop();

		spAnimationState_setAnimation( m_animationState, 0, m_currentAnimation, loop ? 1 : 0 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Spine::_restart( float _time, uint32_t _enumerator )
	{ 
		(void)_time;
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_pause( uint32_t _enumerator )
	{ 
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_resume( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_stop( uint32_t _enumerator )
	{ 
		(void)_enumerator;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_END )(this, _enumerator, false);
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_end( uint32_t _enumerator )
	{ 
		(void)_enumerator;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_SPINE_END )(this, _enumerator, true);
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Spine::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return true;
	}
}