#	pragma once

#	include "NodeProxy.h"

#	include "TrackChain.h"

namespace Menge
{
	class Node;
	class Track;


	struct TrackProxyDesc
	{
		Track *track;
		TListTrackChain::iterator chain;
		float speedMove;
		Node *node;
	};

	class TrackProxy
		: public NodeProxy
	{
	public:
		TrackProxy( const TrackProxyDesc &_desc );

	public:
		void setSpeedMove(float _speed);
		float getSpeedMove()const;

		void setChain(TListTrackChain::iterator _chain);
		TListTrackChain::iterator getChain();

		void start();
		void stop();
		
		bool isMoved()const;

	public:
		void setDirection(const mt::vec2f &dir);

		void setPosition(const mt::vec2f &pos);
		const mt::vec2f & getLocalPosition()const;

	public:
		void update(float _timing) override;

	private:
		bool m_isMove;

		Track *m_track;
		TListTrackChain::iterator m_chain;

		float m_speedMove;
	};
}