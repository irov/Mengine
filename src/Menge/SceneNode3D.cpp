#	include "SceneNode3D.h"

namespace Menge
{
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

		for each( SceneNode3D * children in m_listChildren )
		{
			children->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( TiXmlElement * _xml )
	{
		Allocator3D::loader( _xml );
		Renderable3D::loader( _xml );
		Collideable3D::loader( _xml );

		NodeCore::loader( _xml );
	}
}