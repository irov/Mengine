#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "ResourceSpine.h"

#	include "spine/spine.h"

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
		void onAnimationEvent( spAnimationState * _state, int _trackIndex, spEventType _type, spEvent * _event, int _loopCount );

	protected:
		void fillSlotVertices_( const mt::mat4f & _wm, RenderVertex2D * points, spSlot * slot, spRegionAttachment * attachment );

	protected:
		void updateMaterial_();

	protected:
		ResourceHolder<ResourceSpine> m_resourceSpine;

		ConstString m_currentAnimationName;

		RenderMaterialInterfacePtr m_material;

		spSkeleton * m_skeleton;
		spSkeletonData * m_skeletonData;

		spAnimationStateData * m_animationStateData;
		spAnimationState * m_animationState;

		spAnimation * m_currentAnimation;
		bool m_invalidateCurrentAnimation;
	};
}