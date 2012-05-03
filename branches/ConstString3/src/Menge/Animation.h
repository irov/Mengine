#	pragma once

#	include "Sprite.h"

#	include "Animatable.h"

namespace Menge
{
	class ResourceAnimation;

	class Animation
		: public Sprite
		, public Animatable
	{
	public:
		Animation();
		~Animation();

	public:
		void setAnimationResource( const ConstString& _resource );
		const ConstString& getAnimationResource() const;

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
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		void _interrupt( size_t _enumerator ) override;
	protected:
		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		bool _activate() override;

		bool _compile() override;
		void _release() override;

		void _setEventListener( PyObject * _listener ) override;

	protected:
		size_t getFrame_( float _timing, float & _delthaTiming ) const;
		bool updateCurrentFrame_( size_t _lastFrame );

	protected:
		ResourceAnimation * m_resourceAnimation;
		ConstString m_resourceAnimationName;

		float m_frameTiming;

		size_t m_currentFrame;
		
		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
