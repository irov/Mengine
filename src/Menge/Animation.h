#	pragma once

#	include "Sprite.h"

namespace Menge
{
	enum	eAnimState
	{
		FORWARD,	
		REWIND,	
	};

	class ResourceAnimation;

	class Animation
		: public Sprite
	{
		OBJECT_DECLARE(Animation)
	public:
		Animation();

	public:
		virtual void play();
		virtual void stop();
		virtual void pause();

		virtual void setLooped( bool _looped );
		virtual bool getLooped() const;

		virtual void setFirstFrame();
		virtual void nextFrame();
		virtual void prevFrame();

		virtual void setAnimState( eAnimState _state );
 
	public:
		void setAnimationListener( PyObject * _listener );
		void setAnimationResource( const std::string & _resource );
	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( size_t _timing ) override;
		bool _activate() override;
		void _deactivate() override;

	protected:
		std::string m_resourceAnimation;
		ResourceAnimation * m_animation;

	private:
		size_t	m_total_delay;

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		size_t m_currentFrame;

		eAnimState	m_state;

		PyObject * m_listener;
	};
}