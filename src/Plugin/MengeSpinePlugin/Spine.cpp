#	include "Spine.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Spine::Spine()
		: m_skeleton( nullptr )
		, m_animationStateData( nullptr )
		, m_animationState( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Spine::~Spine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::setResourceSpine( ResourceSpine * _resourceSpine )
	{
		if( m_resourceSpine == _resourceSpine )
		{
			return;
		}

		m_resourceSpine = _resourceSpine;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceSpine * Spine::getResourceSpine() const
	{ 
		return m_resourceSpine;
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

		spSkeleton_update( skeleton, 0.f );
		spAnimationState_update( animationState, 0.f );
		spAnimationState_apply( animationState, skeleton );

		spSkeleton_updateWorldTransform( skeleton );

		m_skeleton = skeleton;
		m_skeletonData = skeletonData;
		m_animationStateData = animationStateData;
		m_animationState = animationState;

		this->updateMaterial_();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_release()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::updateMaterial_()
	{ 
		uint32_t texturesNum = 1;
		RenderTextureInterfacePtr textures[2];

		textures[0] = m_resourceSpine->getAtlasTexture();
		textures[1] = nullptr;

		ConstString stageName = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend" );
		
		m_material = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getMaterial( stageName, false, false, PT_TRIANGLELIST, texturesNum, textures );

		if( m_material == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Spine::updateMaterial_ %s resource %s m_material is NULL"
				, this->getName().c_str()
				, m_resourceSpine->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_spineAnimationStateListener( spAnimationState * _state, int _trackIndex, spEventType _type, spEvent * _event, int _loopCount )
	{
		Spine * spine = (Spine *)_state->rendererObject;

		if( spine != nullptr )
		{
			spine->onAnimationEvent( _state, _trackIndex, _type, _event, _loopCount );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::onAnimationEvent( spAnimationState * _state, int _trackIndex, spEventType _type, spEvent * _event, int _loopCount )
	{
		(void)_loopCount;
		(void)_event;

		spTrackEntry * entry = spAnimationState_getCurrent( _state, _trackIndex );

		if( entry != nullptr && entry->loop > 0 && _type == SP_ANIMATION_COMPLETE )
		{
			this->stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_update( float _current, float _timing )
	{
		if( m_invalidateCurrentAnimation == true )
		{
			m_invalidateCurrentAnimation = false;

			spAnimation * animation = spSkeletonData_findAnimation( m_skeletonData, m_currentAnimationName.c_str() );

			spTrackEntry * entry = spAnimationState_setAnimation( m_animationState, 0, animation, true );

			if( entry != nullptr )
			{
				entry->listener = s_spineAnimationStateListener;
			}

			m_currentAnimation = animation;
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

		float spTiming = _timing * 0.0001f;

		spSkeleton_update( m_skeleton, spTiming );
		spAnimationState_update( m_animationState, spTiming );

		spAnimationState_apply( m_animationState, m_skeleton );

		spSkeleton_updateWorldTransform( m_skeleton );
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		int slotCount = m_skeleton->slotsCount;

		for( int i = 0; i < slotCount; ++i )
		{
			spSlot * slot = m_skeleton->slots[i];

			spAttachment * attachment = slot->attachment;

			if( attachment == nullptr || attachment->type != SP_ATTACHMENT_REGION )
			{
				continue;
			}

			switch( attachment->type )
			{
			case SP_ATTACHMENT_REGION:
				{
					spRegionAttachment * regionAttachment = (spRegionAttachment *)attachment;

					RenderVertex2D * vertices = RENDER_SERVICE( m_serviceProvider )
						->getMutableRenderVertex2D( 4 );

					this->fillSlotVertices_( wm, vertices, slot, regionAttachment );

					RENDER_SERVICE( m_serviceProvider )
						->addRenderQuad( _viewport, _camera, m_material, vertices, 4, nullptr );
				}break;
			default:
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::fillSlotVertices_( const mt::mat4f & _wm, RenderVertex2D * _vertex, spSlot * _slot, spRegionAttachment * _regionAttachment )
	{
		ColourValue color( m_skeleton->r * _slot->r, m_skeleton->g * _slot->g, m_skeleton->b * _slot->b, m_skeleton->a * _slot->a );

		ColourValue_ARGB argb = color.getAsARGB();
				
		_vertex[0].color = argb;
		_vertex[1].color = argb;
		_vertex[2].color = argb;
		_vertex[3].color = argb;

		_vertex[0].uv.x = _regionAttachment->uvs[SP_VERTEX_X1];
		_vertex[0].uv.y = _regionAttachment->uvs[SP_VERTEX_Y1];
		_vertex[1].uv.x = _regionAttachment->uvs[SP_VERTEX_X2];
		_vertex[1].uv.y = _regionAttachment->uvs[SP_VERTEX_Y2];
		_vertex[2].uv.x = _regionAttachment->uvs[SP_VERTEX_X3];
		_vertex[2].uv.y = _regionAttachment->uvs[SP_VERTEX_Y3];
		_vertex[3].uv.x = _regionAttachment->uvs[SP_VERTEX_X4];
		_vertex[3].uv.y = _regionAttachment->uvs[SP_VERTEX_Y4];

		float * offset = _regionAttachment->offset;

		float xx = m_skeleton->x + _slot->bone->worldX;
		float yy = m_skeleton->y + _slot->bone->worldY;

		mt::vec3f pos[4];

		float m00 = _slot->bone->m00;
		float m10 = _slot->bone->m10;
		float m11 = _slot->bone->m11;
		float m01 = _slot->bone->m01;

		pos[0].x = xx + offset[SP_VERTEX_X1] * m00 + offset[SP_VERTEX_Y1] * m01;
		pos[0].y = yy + offset[SP_VERTEX_X1] * m10 + offset[SP_VERTEX_Y1] * m11;
		pos[0].z = 0.f;
		pos[1].x = xx + offset[SP_VERTEX_X2] * m00 + offset[SP_VERTEX_Y2] * m01;
		pos[1].y = yy + offset[SP_VERTEX_X2] * m10 + offset[SP_VERTEX_Y2] * m11;
		pos[1].z = 0.f;
		pos[2].x = xx + offset[SP_VERTEX_X3] * m00 + offset[SP_VERTEX_Y3] * m01;
		pos[2].y = yy + offset[SP_VERTEX_X3] * m10 + offset[SP_VERTEX_Y3] * m11;
		pos[2].z = 0.f;
		pos[3].x = xx + offset[SP_VERTEX_X4] * m00 + offset[SP_VERTEX_Y4] * m01;
		pos[3].y = yy + offset[SP_VERTEX_X4] * m10 + offset[SP_VERTEX_Y4] * m11;
		pos[3].z = 0.f;

		mt::mul_v3_m4( _vertex[0].pos, pos[0], _wm );
		mt::mul_v3_m4( _vertex[1].pos, pos[1], _wm );
		mt::mul_v3_m4( _vertex[2].pos, pos[2], _wm );
		mt::mul_v3_m4( _vertex[3].pos, pos[3], _wm );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Spine::_play( float _time )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Spine::_restart( float _time, uint32_t _enumerator )
	{ 
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_pause( uint32_t _enumerator )
	{ 

	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_resume( uint32_t _enumerator )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_stop( uint32_t _enumerator )
	{ 
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Spine::_end( uint32_t _enumerator )
	{ 
	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Spine::_interrupt( uint32_t _enumerator )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}