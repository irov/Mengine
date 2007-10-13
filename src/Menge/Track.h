#	pragma once

#	include "SceneNode2D.h"

#	include "TrackChain.h"

#	include "Math/vec3.h"

#	include <list>


namespace Menge
{
	class TrackProxy;

	typedef std::list<TrackChain> TListTrackChain;

	class Track
		: public SceneNode2D
	{
		OBJECT_DECLARE(Track);

	public:
		Track();

	public:
		void addPoint(const mt::vec2f & _point);

		void setSpeedMove(float _speed);
		float getSpeedMove()const;

		void start();
		void stop();

		bool isMoved()const;

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		void _debugRender() override;
		void _update( size_t _timing, const Viewport & _viewport )  override;

		bool _activate() override;
	private:
		typedef std::list<mt::vec2f> TListPoint;

		TListPoint m_listPoints;
		TListTrackChain m_listChain;

		TListTrackChain::iterator m_currentChain;

		float m_speedMove;
		bool m_isMove;

		friend class TrackProxy;
	};
}