#	include "Node.h"
#	include "Visitor.h"

#	include "Layer2D.h"

#	include "SceneManager.h"

#	include "RenderEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"
#	include "XmlEngine.h"

#	include "Player.h"
#	include "Camera2D.h"

#	include "pybind/pybind.hpp"

#	include "Config/Config.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_active(false)
		, m_enable(true)
		, m_updatable(true)
		, m_parent(0)
		, m_hide(false)
		, m_visibility(false)
		, m_changeVisibility(true)
		, m_listener(0)
		, m_layer(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Node::checkVisibility( const Viewport & _viewport )
	{
		if( m_changeVisibility )
		{
			m_visibility = _checkVisibility( _viewport );
		}

		return m_visibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::changeVisibility()
	{
		m_changeVisibility = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
		const mt::box2f & bbox = getWorldBoundingBox();

		bool result = _viewport.testRectangle( bbox.vb, bbox.ve );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::destroy()
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->destroy();
		}

		_destroy();

		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visit( Visitor * _visitor )
	{
		_visitor->procces_impl( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			_visitor->procces_impl( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::activate()
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->activate();
		}

		if( m_active )
		{
			return true;
		}

		if( isCompile() == false )
		{
			if( compile() == false )
			{
				MENGE_LOG("Error: activate Node '%s' is failed, becouse compiled is failed\n"
					, m_name.c_str() 
					);

				return false;
			}
		}

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->deactivate();
		}

		if( m_active )
		{
			m_active = false;

			_deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isActivate() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::enable()
	{
		m_enable = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::disable()
	{
		m_enable = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isEnable() const
	{
		return m_enable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setParent( Node * _parent )
	{
		m_parent = _parent;
	}
	//////////////////////////////////////////////////////////////////////////
	Node* Node::getParent()
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren( Node * _node )
	{
		if( isChildren( _node ) )
		{
			MENGE_LOG("Node '%s' type '%s' addChildren failed '%s' type '%s' is already exsist"
				, this->getName().c_str()
				, this->getType().c_str()
				, _node->getName().c_str()
				, _node->getType().c_str()
				);

			return false;
		}

		Node * parent = _node->getParent();

		if( parent )
		{
			parent->removeChildren( _node );
		}

		_node->setParent( this );
		_node->setLayer( m_layer );

		m_childrens.push_back( _node );

		_addChildren( _node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
		TContainerChildrens::iterator it_find = 
			std::find( m_childrens.begin(), m_childrens.end(), _node );

		if( it_find != m_childrens.end() )
		{
			m_childrens.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::getChildren( const std::string & _name, bool _recursion )
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			Node * children = *it;
			if( children->getName() == _name )
			{
				return children;
			}

			if( _recursion )
			{
				if( Node * result = children->getChildren( _name, _recursion ) )
				{
					return result;
				}
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isChildren( Node * _node ) const
	{
		TContainerChildrens::const_iterator it_find = 
			std::find( m_childrens.begin(), m_childrens.end(), _node );

		return it_find != m_childrens.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_addChildren( Node * _node )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_removeChildren( Node * _node )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setUpdatable( bool _updatable )
	{
		m_updatable = _updatable;

		//// !!!! Temporary hack
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->setUpdatable( _updatable );
		}
		//// !!!!
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isUpdatable() const
	{
		/*if( m_updatable == false )
		{
		return false;
		}*/

		if( m_enable == false )
		{
			return false;
		}

		if( m_active == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::update( float _timing )
	{
		if( isUpdatable() == false )
		{
			return;
		}

		if( m_updatable )	// !!!!
		{
			_update( _timing );
		}

		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::loader( XmlElement * _xml )
	{
		Allocator2D::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE("Enable", "Value", m_enable );

			XML_CASE_NODE("Node")
			{
				std::string name;
				std::string type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
					XML_CASE_ATTRIBUTE( "Type", type );
				}

				Node * node = SceneManager::createNode( type );

				if(node == 0)
				{
					continue;
				}

				node->setName( name );
				addChildren( node );

				XML_PARSE_ELEMENT( node, &Node::loader );
			}

			XML_CASE_NODE("Hide")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE("Value", m_hide);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
		//Empty
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_deactivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_update( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::updatable()
	{
		return m_updatable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		bool done = true;

		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			/*done &=*/ (*it)->compile();
		}

		//if( done == false )
		//{
		//	return false;
		//}

		bool result = Resource::compile();

		if( result == false )
		{
			MENGE_LOG("Error: compiled Node '%s' is failed\n"
				, getName().c_str() 
				);

			return false;
		}

		updateBoundingBox();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->release();
		}

		deactivate();

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::recompile()
	{
		//if( isCompile() == false )
		//{
		//	return true;
		//}

		bool status = isActivate();

		release();
		compile();

		if( isCompile() == false )
		{
			return false;
		}

		if( status )
		{
			activate();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
/*	bool Node::checkVisibility( const Viewport & _viewport )
	{
		if( m_changeVisibility )
		{
			m_visibility = _checkVisibility( _viewport );
		}

		return m_visibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::changeVisibility()
	{
		m_changeVisibility = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
		const mt::box2f & bbox = getWorldBoundingBox();

		bool result = _viewport.testRectangle( bbox.vb, bbox.ve );

		return result;
	}*/
	//////////////////////////////////////////////////////////////////////////
	void Node::hide( bool _value )
	{
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isHide() const
	{
		return m_hide;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isRenderable() const
	{
		if( isEnable() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( m_hide == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::render( const Viewport & _viewport )
	{
		if( isRenderable() == false )
		{
			return;
		}

		renderSelf( _viewport );

		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->render( _viewport );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderSelf( const Viewport & _viewport )
	{
		Holder<RenderEngine>::hostage()
			->setRenderViewport( _viewport );

		_render( _viewport, false );

#	ifdef MENGE_DEVELOPMENT
		//_debugRender( _viewport );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_render( const Viewport & _viewport, bool )
	{
		// Empty
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = Holder<ScriptEngine>::hostage()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::changePivot()
	{
		Allocator2D::changePivot();

		this->updateBoundingBox();

		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changePivot()
	{
		Allocator2D::_changePivot();

		m_changeBoundingBox = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & Node::getWorldBoundingBox()
	{
		if( m_childrens.empty() )
		{
			return BoundingBox::getLocalBoundingBox();
		}

		if( isChangeBoundingBox() == false )
		{
			return BoundingBox::getWorldBoundingBox();
		}

		BoundingBox::clearWorldBoundingBox();

		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			const mt::box2f & bbox = (*it)->getWorldBoundingBox();

			BoundingBox::mergeBoundingBox( bbox );
		}

		return BoundingBox::getWorldBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changeBoundingBox() 
	{
		if( m_changeBoundingBox == false )
		{
			m_changeBoundingBox = true;

			if( Node * parent = this->getParent() )
			{
				parent->_changeBoundingBox();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Node::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return Allocator2D::getLocalMatrix();
		}

		if( isChangePivot() == false )
		{
			return Allocator2D::getWorldMatrix();
		}

		const mt::mat3f & wm = m_parent->getWorldMatrix();

		return Allocator2D::updateWorldMatrix( wm );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setListener( PyObject * _listener )
	{
		m_listener = _listener;
		this->_setListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setListener()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer2D * _layer )
	{
		m_layer = _layer;

		// прости господи
		for( TContainerChildrens::iterator
			it = m_childrens.begin(),
			it_end = m_childrens.end();
		it != it_end;
		++it)
		{
			(*it)->setLayer( _layer );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Layer2D * Node::getLayer() const
	{
		return m_layer;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Node::getScreenPosition()
	{
		const mt::vec2f & pos = getWorldPosition();

		mt::vec2f screen_pos;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		if( m_layer )
		{
			const mt::vec2f & factor = m_layer->getParallaxFactor();
			Viewport viewportParallax = viewport;
			viewportParallax.parallax( factor );

			screen_pos = pos - viewportParallax.begin;
		}
		else
		{
			screen_pos = pos - viewport.begin;
		}

		return screen_pos;
	}
}