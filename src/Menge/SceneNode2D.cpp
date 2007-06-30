#	include "SceneNode2D.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"
#	include "ScriptEngine.h"

#	include "Layer2D.h"

#	include "FileEngine.h"
#	include "XmlParser.h"

#	include "ErrorMessage.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D::SceneNode2D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::loader( TiXmlElement * _xml )
	{
		Allocator2D::loader( _xml );
		Renderable2D::loader( _xml );
		NodeCore::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & SceneNode2D::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( m_parent );	
		
		return Allocator2D::getWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::changePivot()
	{
		Allocator2D::changePivot();

		for each( SceneNode2D * children in m_listChildren )
		{
			children->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::isVisible( const Viewport & _viewport )
	{
		const mt::vec2f & pos = getWorldPosition();
		bool res = _viewport.testPoint( pos );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::render( const Viewport & _viewport )
	{
		const Viewport & vp = 
			Renderable2D::updateViewport( _viewport );

		const mt::mat3f & wm = getWorldMatrix();

		renderSelf( wm, vp );

		for each( SceneNode2D * children in m_listChildren )
		{
			children->render( vp );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::debugRender()
	{
		for each( SceneNode2D * children in m_listChildren )
		{
			children->debugRender();
		}

		NodeCore::debugRender();
	}
	


}