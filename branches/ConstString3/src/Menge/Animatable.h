#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Animatable
	{
	public:
		Animatable();

	public:
		void setLoop( bool _value );
		bool getLoop() const;

		void setSpeedFactor( float _factor );
		float getSpeedFactor() const;

		void setStartInterval( float _startInterval );
		float getStartInterval( ) const;
		
		void setLastFrame();
		void setFirstFrame();

		void setTiming( float _timing );
		float getTiming() const;

		void setReverse( bool _value);
		bool getReverse() const;

	public:
		virtual void _setTiming( float _timing );
		virtual float _getTiming() const;

		virtual void _setSpeedFactor( float _factor );
		virtual void _setFirstFrame();
		virtual void _setLastFrame();

		virtual void _setReverse( bool _value );

	public:
		size_t play( float _time );
		bool stop();
		bool interrupt();
		void end();

	public:
		bool isPlay() const;
		size_t getPlayId() const;

		float getPlayTime() const;

	protected:
		virtual bool _play() = 0;
		virtual bool _restart( size_t _enumerator ) = 0;
		virtual void _stop( size_t _enumerator ) = 0;
		virtual void _end( size_t _enumerator ) = 0;
		virtual bool _interrupt( size_t _enumerator ) = 0;

	protected:
		virtual void _setLoop( bool _value );

	protected:
		size_t m_enumerator;
		
		float m_playTime;

		float m_speedFactor;
		float m_startInterval;
		
		bool m_play;

		bool m_loop;
		bool m_reverse;
	};
}