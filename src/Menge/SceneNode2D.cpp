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
	void SceneNode2D::_render()
	{
		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->render();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::debugRender()
	{
		NodeCore::debugRender();

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->debugRender();
		}		
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
			Viewport * viewport = m_layer->getViewport();
			pos -= viewport->begin;

			// if we have case with 2 viewports, check in what viewport we see point (looks more like a hack)
			if( m_layer->isScrollable() && pos.x > viewport->end.x )
			{
				pos -= m_layer->getViewportOffset();
			}
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