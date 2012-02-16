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

	public:
		size_t play();
		bool stop();
		
		void end();
		
	public:
		bool isPlay() const;
		size_t getPlayId() const;

	protected:
		virtual bool _play() = 0;
		virtual bool _restart( size_t _enumerator ) = 0;
		virtual void _stop( size_t _enumerator ) = 0;
		virtual void _end( size_t _enumerator ) = 0;

	protected:
		virtual void _setLoop( bool _value );

	protected:
		size_t m_enumerator;
		bool m_play;

		bool m_loop;
	};
}