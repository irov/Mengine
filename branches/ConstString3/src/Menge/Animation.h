#	pragma once

#	include "Sprite.h"

#	include "Animatable.h"

namespace Menge
{
	class ResourceSequence;

	class Animation
		: public Sprite
		, public Animatable
	{
	public:
		Animation();
		~Animation();

	public:
		void setSequenceResource( const ConstString& _resource );
		const ConstString& getSequenceResource() const;

		void setAnimationFactor( float _factor );
		float getAnimationFactor() const;

		size_t getFrameCount() const;
		float getFrameDelay( size_t  _frame ) const;
		
		void setCurrentFrame( size_t _frame );
		size_t getCurrentFrame() const;

		void setTimming( float _timming );
		float getTimming() const;

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
		void updateCurrentImageIndex_();

	protected:
		ResourceSequence * m_resourceSequence;
		ConstString m_resourceSequenceName;
	
		float m_timinig;

		size_t m_currentFrame;

		float m_animationFactor;

		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
