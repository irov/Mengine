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
		inline bool getLoop() const;

		void setAnimationSpeedFactor( float _factor );
		inline float getAnimationSpeedFactor() const;

		void setIntervalStart( float _startInterval );
		inline float getIntervalStart() const;

        void setInterval( float _begin, float _end );
        inline float getIntervalBegin() const;
        inline float getIntervalEnd() const;
		
		void setFirstFrame();
		void setLastFrame();		

		void setTiming( float _timing );
		float getTiming() const;

        void setPlayCount( uint32_t _count );
        inline uint32_t getPlayCount() const;

		inline uint32_t getPlayIterator() const;

		void setAutoPlay( bool _autoPlay );
		inline bool getAutoPlay() const;

		void setStretch( float _scretch );
        inline float getStretch() const;

    protected:
        virtual void _setLoop( bool _value );
		virtual void _setTiming( float _timing );
		virtual float _getTiming() const;

		virtual void _setAnimationSpeedFactor( float _factor );
		virtual void _setFirstFrame();
		virtual void _setLastFrame();

	public:
		uint32_t play( float _time );
		bool stop();
		bool pause();
		void resume( float _time );
		bool interrupt();

	public:
		void update( float _timing );

	public:
		inline bool isPlay() const;
		inline bool isPause() const;

	public:
		inline bool isInterrupt() const;
		
	public:
		inline bool isLastCicle() const;

	protected:
		void end();

	public:
		inline uint32_t getPlayId() const;
		inline float getPlayTime() const;

	protected:
		virtual bool _play( float _time ) = 0;
		virtual bool _restart( float _time, uint32_t _enumerator ) = 0;
		virtual void _pause( uint32_t _enumerator ) = 0;
		virtual void _resume( float _time, uint32_t _enumerator ) = 0;
		virtual void _stop( uint32_t _enumerator ) = 0;
		virtual void _end( uint32_t _enumerator ) = 0;
		virtual bool _interrupt( uint32_t _enumerator ) = 0;

	protected:
		uint32_t m_enumerator;
		
		float m_playTime;

		float m_animationSpeedFactor;        
		float m_intervalStart;
        float m_intervalBegin;
        float m_intervalEnd;
        
        uint32_t m_playCount;
		uint32_t m_playIterator;
        float m_stretch;
		
		bool m_autoPlay;

		bool m_play;
		bool m_pause;
		bool m_interrupt;

		bool m_loop;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::isPlay() const
	{
		return m_play == true && m_pause == false;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::isPause() const
	{
		return m_pause;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::getLoop() const
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getAnimationSpeedFactor() const
	{
		return m_animationSpeedFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t Animatable::getPlayCount() const
	{
		return m_playCount;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t Animatable::getPlayIterator() const
	{
		return m_playIterator;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getStretch() const
	{
		return m_stretch;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::isInterrupt() const
	{
		return m_interrupt;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::isLastCicle() const
	{
		if( m_interrupt == true )
		{
			return true;
		}

		if( m_loop == true )
		{
			return false;
		}

		if( m_playIterator > 1 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t Animatable::getPlayId() const
	{
		return m_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getPlayTime() const
	{
		return m_playTime;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getIntervalStart() const
	{
		return m_intervalStart;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getIntervalBegin() const
	{
		return m_intervalBegin;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Animatable::getIntervalEnd() const
	{
		return m_intervalEnd;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Animatable::getAutoPlay() const
	{ 
		return m_autoPlay;
	}
}