#	include "BoundingBox.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BoundingBox::BoundingBox()
	{
		mt::reset( m_localBoundingBox, 0.f, 0.f );
		mt::reset( m_worldBoundingBox, 0.f, 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & BoundingBox::getLocalBoundingBox() const
	{
		return m_localBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::setLocalBoundingBox( const mt::box2f & _box )
	{
		m_localBoundingBox = _box;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & BoundingBox::getWorldBoundingBox() const
	{
		return m_worldBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BoundingBox::isChangeBoundingBox() const
	{
		return m_changeBoundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::mergeBoundingBox( const mt::box2f & _bbox )
	{
		mt::merge_box( m_worldBoundingBox, _bbox );
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_changeBoundingBox() 
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::clearWorldBoundingBox()
	{
		_updateBoundingBox( m_localBoundingBox );

		m_worldBoundingBox = m_localBoundingBox;
		m_changeBoundingBox = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::updateBoundingBox()
	{
		//_updateBoundingBox( m_localBoundingBox );

		_changeBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void BoundingBox::_updateBoundingBox( mt::box2f & _localBoundingBox )
	{
		//Empty
	}
}