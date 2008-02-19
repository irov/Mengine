#	include "SceneNode2D.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "Layer2D.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D::SceneNode2D()
		: m_layer( NULL )
		, m_changePivot( true )
	{
		mt::ident_m3( m_localMatrix );
		mt::ident_m3( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::loader( XmlElement * _xml )
	{
		NodeCore::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &SceneNode2D::setLocalMatrix );
					XML_CASE_ATTRIBUTE_MEMBER( "Position", &SceneNode2D::setLocalPosition );
					XML_CASE_ATTRIBUTE_MEMBER( "Direction", &SceneNode2D::setLocalDirection );
					XML_CASE_ATTRIBUTE_MEMBER( "Rotate", &SceneNode2D::setRotate );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::isRenderable()
	{
		if( NodeCore::isRenderable() == false )
		{
			return false;
		}
		
		const Viewport & viewport =
			Holder<RenderEngine>::hostage()
			->getRenderViewport();

		if( isVisible( viewport ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & SceneNode2D::getWorldMatrix()
	{
		//убрать в Node!! тогда никакого dynamic_cast
		SceneNode2D * parent = dynamic_cast<SceneNode2D*>(m_parent);

		if( parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( parent );	

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::changePivot()
	{
		_changePivot();

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			//убрать в Node!! тогда никакого dynamic_cast
			SceneNode2D * children = dynamic_cast<SceneNode2D*>(*it);
			children->changePivot();
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
			if( m_layer->isScrollable() && ( pos.x > viewport->end.x || pos.x < viewport->begin.x ))
			{
				pos -= m_layer->getViewportOffset();
			}

		}

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::_addChildren( Node * _node )
	{
		dynamic_cast<SceneNode2D*>(_node)->setLayer( m_layer );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::update( float _timing )
	{
		NodeCore::update( _timing );
		if( m_layer && m_layer->isScrollable() )
		{
			mt::vec2f& pos = getLocalPositionModify();
			if( pos.x > m_layer->getSize().x )
			{
				pos.x -= m_layer->getSize().x;
			}
			else if( pos.x < 0 )
			{
				pos.x += m_layer->getSize().x;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & SceneNode2D::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & SceneNode2D::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & SceneNode2D::getLocalPosition()const
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & SceneNode2D::getLocalDirection()const
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & SceneNode2D::getLocalMatrix()const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & SceneNode2D::getLocalPositionModify()
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & SceneNode2D::getLocalDirectionModify()
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat3f & SceneNode2D::getLocalMatrixModify()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setLocalMatrix( const mt::mat3f & _matrix )
	{
		m_localMatrix = _matrix;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setLocalPosition( const mt::vec2f & _position )
	{
		mt::vec2f & localPosition = getLocalPositionModify();

		localPosition = _position;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setLocalDirection( const mt::vec2f & _direction )
	{
		m_localMatrix.v0.v2 = _direction;
		m_localMatrix.v1.v2 = mt::perp( _direction );

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setRotate( float _alpha )
	{
		float cos_alpha = cosf(_alpha);
		float sin_alpha = sinf(_alpha);
		m_localMatrix[0][0] = cos_alpha;
		m_localMatrix[0][1] = -sin_alpha;
		m_localMatrix[1][0] = sin_alpha;
		m_localMatrix[1][1] = cos_alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::translate( const mt::vec2f & _delta )
	{
		m_localMatrix.v2.v2 += _delta;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::updateMatrix( SceneNode2D * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}
	
		const mt::mat3f & parentMatrix =
			_parent->getWorldMatrix();
	
		mt::mul_m3_m3( m_worldMatrix, m_localMatrix, parentMatrix );

		_updateMatrix( _parent );
	
		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::_updateMatrix( SceneNode2D * _parent )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::_changePivot()
	{
		m_changePivot = true;
	}
}