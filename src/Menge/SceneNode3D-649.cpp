#	include "SceneNode3D-649.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & SceneNode3D_::getWorldMatrix()
	{
		/*if( m_parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( m_parent );	*/

		return Allocator3D_::getWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::changePivot()
	{
		Allocator3D_::changePivot();

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
	void SceneNode3D_::loader( XmlElement * _xml )
	{
		Allocator3D_::loader( _xml );
		Renderable3D::loader( _xml );

		NodeCore::loader( _xml );
	}
}