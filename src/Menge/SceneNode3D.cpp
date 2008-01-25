#	include "SceneNode3D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & SceneNode3D::getWorldOrient()
	{
		if( m_parent == 0 )
		{
			return getLocalOrient();
		}

		updateOrient( m_parent );	

		return Allocator3D::getWorldOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getWorldPosition()
	{
		if( m_parent == 0 )
		{
			return getLocalPosition();
		}

		updatePosition( m_parent );	

		return Allocator3D::getWorldPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::changePivot()
	{
		Allocator3D::changePivot();

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		Allocator3D::loader( _xml );
		Renderable3D::loader( _xml );
		NodeCore::loader( _xml );
	}
}