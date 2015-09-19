#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "ResourceSpine.h"

#	pragma warning (disable:4510)
#	pragma warning (disable:4512)
#	pragma warning (disable:4610)

#	include "spine/spine.h"

#	include <stdex/heap_array.h>

namespace Menge
{
	class Spine
		: public Node
		, public Animatable
	{
	public:
		Spine();
		~Spine();

	public:
		void setResourceSpine( ResourceSpine * _resourceSpine );
		ResourceSpine * getResourceSpine() const;

	public:
		void setAnimationName( const ConstString & _name );
		const ConstString & getAnimationName() const;

	public:
		void setAnimationMix( const ConstString & _from, const ConstString & _to, float _time );

	public:
		float getDuration( const ConstString & _name );

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	public:
		void addAnimationEvent( int _trackIndex, spEventType _type, spEvent * _event, int _loopCount );

	protected:
		void fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColourValue_ARGB _argb, int _count, const mt::mat4f & _wm );
		void fillIndices_( RenderIndices * _vertices2D, const int * _triangles, int _count );
		
	protected:
		RenderMaterialInterfacePtr makeMaterial_( spSlot * slot, ResourceImage * _resourceImage ) const;

	protected:
		void updateAnimation_();

	protected:
		ResourceHolder<ResourceSpine> m_resourceSpine;
			
		spSkeleton * m_skeleton;

		spAnimationStateData * m_animationStateData;
		spAnimationState * m_animationState;

		struct AttachmentMesh
		{
			RenderMaterialInterfacePtr material;

			typedef stdex::heap_array<RenderVertex2D> TArrayRenderVertex2D;
			TArrayRenderVertex2D vertices;

			typedef stdex::heap_array<RenderIndices> TArrayRenderIndices;
			TArrayRenderIndices indices;
		};

		typedef stdex::vector<AttachmentMesh> TVectorAttachmentMesh;
		TVectorAttachmentMesh m_attachmentMeshes;

		ConstString m_currentAnimationName;
		spAnimation * m_currentAnimation;

		struct AnimationEvent
		{
			int trackIndex;
			spEventType type;

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