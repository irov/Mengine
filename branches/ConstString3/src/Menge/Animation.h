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

		std::size_t getCurrentFrame() const;
		std::size_t getFrameCount() const;

		float getFrameDelay( std::size_t  _frame ) const;
		void setCurrentFrame( std::size_t _frame );

	protected:
		bool _play() override;
		void _restart( std::size_t _enumerator ) override;
		void _stop( std::size_t _enumerator ) override;
		void _end( std::size_t _enumerator ) override;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;

		bool _compile() override;
		void _release() override;

		void _setEventListener( PyObject * _listener ) override;
		
	protected:
		ResourceSequence * m_resourceSequence;
		ConstString m_resourceSequenceName;
	
		float m_timinig;

		std::size_t m_currentFrame;

		float m_animationFactor;

		bool m_onEndFrameEvent;
		bool m_onEndFrameTick;
		bool m_onEndAnimationEvent;
	};
}
