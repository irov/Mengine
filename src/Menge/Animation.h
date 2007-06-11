#	pragma once

#	include "Sprite.h"

namespace Menge
{
	enum	eAnimState
	{
		FORWARD,	
		REWIND,	
		//PINGPONG,	//надо ли нам такой стейт? т.е. анимация играется сначало вперед, потом назад.
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

		void setFirstFrame();
		void nextFrame();
		void prevFrame();

	protected:
		void _update(float _timing) override;
		bool _activate() override;

	private:
		float	m_total_delay;

		bool m_playing;
		bool m_looping;

		eAnimState	m_state;
	};
}