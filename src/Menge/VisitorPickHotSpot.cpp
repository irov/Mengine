#	include "VisitorPickHotSpot.h"

#	include "HotSpot.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VisitorPickHotSpot::VisitorPickHotSpot( const mt::vec2f & _pickPosition, bool _topStop )
		: m_pickPosition( _pickPosition )
		, m_topSpot( _topStop )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	TListHotSpot & VisitorPickHotSpot::getList()
	{
		return m_listHotSpot;
	}
	//////////////////////////////////////////////////////////////////////////
	const TListHotSpot & VisitorPickHotSpot::getList()const
	{
		return m_listHotSpot;
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot * VisitorPickHotSpot::top()
	{
		if( m_listHotSpot.empty() == true )
		{
			return 0;
		}

		return m_listHotSpot.front();
	}
	//////////////////////////////////////////////////////////////////////////
	void VisitorPickHotSpot::mask_call( HotSpot *_node)
	{		
		bool res = _node->testPoint( m_pickPosition );

		if( res == true )
		{
			m_listHotSpot.push_back( _node );

			if( m_topSpot )
			{
				//TODO: break and exit;
			}
		}

		_node->visitChildren( this );
	}
}