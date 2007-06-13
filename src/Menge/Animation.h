#	pragma once

#	include "Sprite.h"
#	include "ResourceAnimation.h"

namespace Menge
{
	enum	eAnimState
	{
		FORWARD,	
		REWIND,	
	};

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

		virtual void setLooped(bool _looped);
		virtual bool getLooped() const;

		virtual void setFirstFrame();
		virtual void nextFrame();
		virtual void prevFrame();

		virtual void setAnimState(eAnimState _state);

	public:
		void loader(TiXmlElement *xml) override;

	protected:
		void _update(float _timing) override;
		bool _activate() override;
		void _deactivate() override;

	protected:
		std::string m_resourceAnim;
		ResourceAnimation * m_anim;
		size_t m_currentFrame;

	private:
		float	m_total_delay;

		bool	m_playing;
		bool	m_looping;

		eAnimState	m_state;
	};
}