#	pragma once

#	include "Sprite.h"

#	include "Kernel/Animatable.h"

#   include "ResourceAnimation.h"

namespace Menge
{
	class Animation
		: public Sprite
		, public Animatable
	{
	public:
		Animation();
		~Animation();

	public:
		void setResourceAnimation( ResourceAnimation * _resourceAnimation );
		ResourceAnimation * getResourceAnimation() const;

		size_t getFrameCount() const;
		float getFrameDelay( size_t  _frame ) const;

		void setCurrentFrame( size_t _frame );
		size_t getCurrentFrame() const;

	protected:
		void _setTiming( float _timming ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

		void _setReverse( bool _value ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( RenderCameraInterface * _camera ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _setEventListener( PyObject * _listener ) override;

	protected:
		size_t getFrame_( float _timing, float & _delthaTiming ) const;
		void updateCurrentFrame_();

	protected:
		ResourceHolder<ResourceAnimation> m_resourceAnimation;

		float m_frameTiming;

		size_t m_currentFrame;
		
		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
