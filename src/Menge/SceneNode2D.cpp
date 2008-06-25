#	include "SceneNode2D.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "Layer2D.h"

#	include "FileEngine.h"
#	include "Application.h"

#	include "XmlEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D::SceneNode2D()
		: m_layer( NULL )
	//	, m_listener( NULL )
	{}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::loader( XmlElement * _xml )
	{
		NodeCore::loader( _xml );
		Allocator2D::loader( _xml );
		Renderable2D::loader( _xml );
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
	void SceneNode2D::_render( bool _enableDebug )
	{
		/*for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->render();
		}*/
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
			if( m_layer->needReRender() )
			{
				const mt::vec2f& screen = Holder<Application>::hostage()->getCurrentResolution();
				if( pos.x >= 0.0f && pos.x <= screen.x )
				{
					return pos;
				}
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
	/*void SceneNode2D::update( float _timing )
	{
		if( m_layer && m_layer->isScrollable() )
		{
			const mt::vec2f& pos = getWorldPosition();
			if( pos.x > m_layer->getSize().x )
			{
				//pos.x -= m_layer->getSize().x;
				translate( mt::vec2f( -m_layer->getSize().x, 0.0f ));
			}
			else if( pos.x < 0 )
			{
				//pos.x += m_layer->getSize().x;
				translate( mt::vec2f( m_layer->getSize().x, 0.0f ));
			}
			//changePivot();
		}
		NodeCore::update( _timing );
	}*/
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setListener( PyObject* _listener )
	{
		//m_listener = _listener;
		//this->_onSetListener();
		Eventable::setListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	/*void SceneNode2D::_onSetListener()
	{
	}*/
	//////////////////////////////////////////////////////////////////////////
	Layer2D* SceneNode2D::getLayer() const
	{
		return m_layer;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::alphaTo( float _alpha, float _time )
	{
		for( TListChildren::iterator it = m_listChildren.begin(),
				it_end = m_listChildren.end();
				it != it_end;
			it++ )
		{
			static_cast<SceneNode2D*>(*it)->alphaTo( _alpha, _time );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setAlpha( float _alpha )
	{
		for( TListChildren::iterator it = m_listChildren.begin(),
				it_end = m_listChildren.end();
				it != it_end;
			it++ )
		{
			static_cast<SceneNode2D*>(*it)->setAlpha( _alpha );
		}
	}
}