#	pragma once

#	include "Sprite.h"

namespace Menge
{
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

	public:
		void setAnimationListener( PyObject * _listener );
		void setAnimationResource( const std::string & _resource );

	public:
		void setAnimationFactor( float _factor );
		float getAnimationFactor() const;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	private:
		void play_();

	protected:
		std::string m_resourceAnimation;
		ResourceAnimation * m_animation;
	
		float m_total_delay;

		bool m_autoStart;
		bool m_playing;
		bool m_looping;
		size_t m_currentFrame;

		float m_animationFactor;

		PyObject * m_listener;
	};
}