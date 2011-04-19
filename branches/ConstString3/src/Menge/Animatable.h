#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Animatable
	{
	public:
		Animatable();

	public:
		std::size_t play();
		bool stop();

		void end();

	public:
		bool isPlay() const;

	protected:
		virtual bool _play() = 0;
		virtual void _stop( std::size_t _enumerator ) = 0;
		virtual void _end( std::size_t _enumerator ) = 0;

	protected:
		std::size_t m_enumerator;
		bool m_play;
	};
}