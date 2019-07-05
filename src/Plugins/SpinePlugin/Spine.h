#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "ResourceSpine.h"

#	ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning (disable:4510)
#	pragma warning (disable:4512)
#	pragma warning (disable:4610)
#	endif

#	include "spine/spine.h"

#	include <stdex/heap_array.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum SpineEventFlag
    {
        EVENT_SPINE_EVENT = __EVENT_ANIMATABLE_LAST__,
        EVENT_SPINE_STATE_ANIMATION_END
    };
    //////////////////////////////////////////////////////////////////////////
    class SpineEventReceiver
        : public AnimatableEventReceiver
    {
    public:
        virtual void onSpineEvent( const char * _eventName, int _eventIntValue, float _eventFloatValue, const char * _eventStringValue ) = 0;
        virtual void onSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class Spine
		: public Node
		, public Eventable
		, public Animatable
	{
        EVENT_RECEIVER( SpineEventReceiver );

	public:
		Spine();
		~Spine();

	public:
		void setResourceSpine( const ResourceSpinePtr & _resourceSpine );
		const ResourceSpinePtr & getResourceSpine() const;

	public:
		bool mixAnimation( const ConstString & _first, const ConstString & _second, float _mix );

	public:
		bool setStateAnimation( const ConstString & _state, const ConstString & _name, float _timing, float _speedFactor, bool _loop );
		bool removeStateAnimation( const ConstString & _state );

	public:
		bool setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor );
		float getStateAnimationSpeedFactor( const ConstString & _state ) const;

		bool setStateAnimationTiming( const ConstString & _state, float _timing );
		float getStateAnimationTiming( const ConstString & _state ) const;

		bool setStateAnimationFreeze( const ConstString & _state, bool _freeze );
		bool getStateAnimationFreeze( const ConstString & _state ) const;

		float getStateAnimationDuration( const ConstString & _state ) const;
		
	public:
		float getAnimationDuration( const ConstString & _name );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _resume ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	public:
		void addAnimationEvent( spAnimationState * _state, int _trackIndex, spEventType _type, spEvent * _event, int _loopCount );

	protected:
		void fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColourValue_ARGB _argb, int _count, const mt::mat4f & _wm );
		void fillIndices_( RenderIndices * _vertices2D, const int * _triangles, int _count );
		
	protected:
		RenderMaterialInterfacePtr makeMaterial_( spSlot * _slot, ResourceImage * _resourceImage ) const;

	protected:		
		void updateAnimation_();

	protected:
		ResourceHolder<ResourceSpine> m_resourceSpine;
			
		spSkeleton * m_skeleton;
		spAnimationStateData * m_animationStateData;

		struct Animation
		{
			ConstString name;
			spAnimationState * state;
			float timing;
			float duration;
			float speedFactor;
			bool freeze;
			bool complete;
			bool loop;
		};

		typedef stdex::map<ConstString, Animation> TMapAnimations;
		TMapAnimations m_animations;

		struct AttachmentMesh
		{
			ResourceImagePtr image;
			RenderMaterialInterfacePtr material;

			typedef stdex::heap_array<RenderVertex2D> TArrayRenderVertex2D;
			TArrayRenderVertex2D vertices;

			typedef stdex::heap_array<RenderIndices> TArrayRenderIndices;
			TArrayRenderIndices indices;
		};

		typedef stdex::map<const char *, AttachmentMesh> TVectorAttachmentMesh;
		TVectorAttachmentMesh m_attachmentMeshes;

		struct AnimationEvent
		{
			int trackIndex;
			spEventType type;

			ConstString state;
			ConstString animation;

			const char * eventName;
			int eventIntValue;
			float eventFloatValue;
			const char* eventStringValue;

			int loopCount;
		};

		typedef stdex::vector<AnimationEvent> TVectorAnimationEvent;
		TVectorAnimationEvent m_events;
	};
}