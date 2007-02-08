#	pragma once

#	include "VisitorMask.h"

#	include "Math/vec2.h"

#	include <list>

namespace Menge
{
	class HotSpot;

	typedef std::list<HotSpot*> TListHotSpot;

	class VisitorPickHotSpot
		: public VisitorMask<HotSpot>
	{		
	public:
		VisitorPickHotSpot( const mt::vec2f & _pickPosition, bool _topSpot = true );

	public:
		TListHotSpot & getList();
		const TListHotSpot & getList()const;

		HotSpot * top();

	protected:
		void mask_call( HotSpot *_node);
	
	private:
		mt::vec2f m_pickPosition;
		TListHotSpot m_listHotSpot;
		bool m_topSpot;
	};
}