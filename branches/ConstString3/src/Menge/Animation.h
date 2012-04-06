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

		void setTiming( float _timming );
		float getTiming() const;

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;

		bool _compile() override;
		void _release() override;

		void _setEventListener( PyObject * _listener ) override;

	protected:
		bool updateCurrentImageResource_();

	protected:
		ResourceAnimation * m_resourceAnimation;
		ConstString m_resourceAnimationName;

		float m_timing;

		size_t m_currentFrame;
		
		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
