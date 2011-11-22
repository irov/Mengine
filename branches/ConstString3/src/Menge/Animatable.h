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
		std::size_t play();
		bool stop();
		
		void end();
		
	public:
		bool isPlay() const;

	protected:
		virtual bool _play() = 0;
		virtual bool _restart( std::size_t _enumerator ) = 0;
		virtual void _stop( std::size_t _enumerator ) = 0;
		virtual void _end( std::size_t _enumerator ) = 0;

	protected:
		virtual void _setLoop( bool _value );

	protected:
		std::size_t m_enumerator;
		bool m_play;

		bool m_loop;
	};
}