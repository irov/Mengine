#	pragma once

#	include "Allocator.h"

#	include "TrackChain.h"

#	include <list>

#	include "Math/vec3.h"

namespace Menge
{
	class TrackProxy;

	class Track
		: public Allocator
	{
		OBJECT_DECLARE(Track);

	public:
		Track();

	public:
		void addPoint(const mt::vec3f &_point);

		void setSpeedMove(float _speed);
		float getSpeedMove()const;

		void start();
		void stop();

		bool isMoved()const;

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
		void _update( float _timing)  override;

		bool _compile() override;

	private:
		

	private:
		typedef std::list<mt::vec3f> TListPoint;

		TListPoint m_listPoints;
		TListTrackChain m_listChain;

		TListTrackChain::iterator m_currentChain;

		float m_speedMove;
		bool m_isMove;

		friend class TrackProxy;
	};
}