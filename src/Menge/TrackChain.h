#	pragma once

#	include <list>

#	include "Math/vec3.h"

namespace Menge
{
	class TrackChain
	{
	public:
		TrackChain(mt::vec3f *_begin, mt::vec3f * _end);

	public:
		const mt::vec3f & beginPoint()const;
		const mt::vec3f & endPoint()const;

		const mt::vec3f & direction()const;
		float length()const;

	private:
		mt::vec3f *m_beginPoint;
		mt::vec3f *m_endPoint;

		mt::vec3f m_direction;
		float m_length;
	};

	typedef std::list<TrackChain> TListTrackChain;
}