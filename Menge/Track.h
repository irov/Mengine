#	pragma once

#	include "Allocator.h"

#	include "TrackChain.h"

#	include <list>

#	include "Math/vec2.h"

namespace RvEngine
{
	class TrackProxy;

	class Track
		: public Allocator
	{
		OBJECT_DECLARE(Track);

	public:
		void addPoint(const mt::vec2f &_point);
		
	public:
		bool addChildren(Node *_node) override;
		
		void loader(TiXmlElement *_xml) override;

		void _debugRender() override;

	private:
		void updateTrackNode(TrackProxy *_trackProxy, float _timing);

	private:
		typedef std::list<mt::vec2f> TListPoint;

		TListPoint m_listPoints;
		TListTrackChain m_listChain;

		friend class TrackProxy;
	};
}