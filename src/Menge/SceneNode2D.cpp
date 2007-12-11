#	include "SceneNode2D.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "Layer2D.h"

#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D::SceneNode2D()
		: m_layer( NULL )
	{}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::loader( TiXmlElement * _xml )
	{
		NodeCore::loader( _xml );
		Allocator2D::loader( _xml );
		Renderable2D::loader( _xml );
		Collideable2D::loader( _xml );
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
	void SceneNode2D::_render()
	{
		for each( SceneNode2D * children in m_listChildren )
		{
			children->render();
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
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setLayer( Layer2D* _layer )
	{
		m_layer = _layer;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f SceneNode2D::getScreenPosition()
	{
		mt::vec2f pos = getWorldPosition();

		if( m_layer )
		{
			pos -= m_layer->getViewport()->begin;
		}

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::_addChildren( SceneNode2D * _node )
	{
		_node->setLayer( m_layer );
	}
	//////////////////////////////////////////////////////////////////////////
}