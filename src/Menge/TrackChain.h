#	pragma once

#	include <list>

#	include "Math/vec2.h"

namespace RvEngine
{
	class TrackChain
	{
	public:
		TrackChain(mt::vec2f *_begin, mt::vec2f * _end);

	public:
		const mt::vec2f & beginPoint()const;
		const mt::vec2f & endPoint()const;

		const mt::vec2f & direction()const;
		float length()const;

	private:
		mt::vec2f *m_beginPoint;
		mt::vec2f *m_endPoint;

		mt::vec2f m_direction;
		float m_length;
	};

	typedef std::list<TrackChain> TListTrackChain;
}