#	include "SceneNode3D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & SceneNode3D::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( m_parent );	

		return Allocator3D::getWorldMatrix();
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