#	pragma once

#	include "Config/Typedef.h"

#   include "Math/vec2.h"

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

		void setIntervalStart( float _startInterval );
		float getStartInterval() const;

        void setInterval( float _begin, float _end );
        float getIntervalBegin() const;
        float getIntervalEnd() const;
		
		void setFirstFrame();
		void setLastFrame();		

		void setTiming( float _timing );
		float getTiming() const;

		void setReverse( bool _value);
		bool getReverse() const;

        void setPlayCount( size_t _count );
        size_t getPlayCount() const;

    protected:
        virtual void _setLoop( bool _value );
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
		virtual bool _play( float _time ) = 0;
		virtual bool _restart( float _time, size_t _enumerator ) = 0;
		virtual void _stop( size_t _enumerator ) = 0;
		virtual void _end( size_t _enumerator ) = 0;
		virtual bool _interrupt( size_t _enumerator ) = 0;		

	protected:
		size_t m_enumerator;
		
		float m_playTime;

		float m_speedFactor;
		float m_intervalStart;
        float m_intervalBegin;
        float m_intervalEnd;

        size_t m_playCount;
		
		bool m_play;

		bool m_loop;
		bool m_reverse;
	};
}