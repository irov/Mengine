#	include "Node.h"
#	include "Visitor.h"

#	include "Layer2D.h"

#	include "SceneManager.h"

#	include "RenderEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"
#	include "XmlEngine.h"
#	include "Application.h"

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
		, m_colorLocal( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_colorWorld( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_invalidateColor( true )
	{}
	//////////////////////////////////////////////////////////////////////////
	void Node::destroy()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
		// Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visit( Visitor * _visitor )
	{
		_visitor->visit_impl( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->visit( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::activate()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
				/*MENGE_LOG( MENGE_TEXT("Error: activation of Node '%s' is failed, because compilation is failed\n")
					, m_name.c_str() 
					);*/

				return false;
			}
		}

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
		if( isChildren( _node, false ) )
		{
			MENGE_LOG_ERROR( "Node \"%s\" type \"%s\" addChildren failed \"%s\" because type \"%s\" is already exist"
			, this->getName().c_str()
			, this->getType().c_str()
			, _node->getName().c_str()
			, _node->getType().c_str() );
			return false;
		}

		Node * parent = _node->getParent();

		if( parent )
		{
			parent->removeChildren( _node );
		}

		_node->setParent( this );
		_node->setLayer( m_layer );

		m_children.push_back( _node );

		_addChildren( _node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenFront( Node* _node )
	{
		if( isChildren( _node, false ) )
		{
			MENGE_LOG_ERROR( "Node \"%s\" type \"%s\" addChildren failed \"%s\" because type \"%s\" is already exist"
			, this->getName().c_str()
				, this->getType().c_str()
				, _node->getName().c_str()
				, _node->getType().c_str() );
			return false;
		}

		Node * parent = _node->getParent();

		if( parent )
		{
			parent->removeChildren( _node );
		}

		_node->setParent( this );
		_node->setLayer( m_layer );

		m_children.push_front( _node );

		_addChildren( _node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
		TContainerChildren::iterator it_find = 
			std::find( m_children.begin(), m_children.end(), _node );

		if( it_find != m_children.end() )
		{
			(*it_find)->setParent( 0 );
			m_children.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::getChildren( const String& _name, bool _recursion ) const
	{
		for( TContainerChildren::const_iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
	bool Node::isChildren( Node * _node, bool _recursive ) const
	{
		TContainerChildren::const_iterator it_find = 
			std::find( m_children.begin(), m_children.end(), _node );

		bool found = ( it_find != m_children.end() );
		if( !found && _recursive )
		{
			for( TContainerChildren::const_iterator it = m_children.begin(), it_end = m_children.end();
				it != it_end;
				it++ )
			{
				if( (*it)->isChildren( _node, _recursive ) == true )
				{
					return true;
				}
			}
		}
		return found;
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

		//// !!!! ÀÕÒÓÍÃ, ÀËÀÐÌ!! ÂÐÎÒÌÍÅÍÎÃÈ, ôàê ìîé ÌÎÑÊ
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		/*++it*/)
		{
			(*it++)->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::loader( XmlElement * _xml )
	{
		Allocator2D::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Enable", "Value", m_enable );

			XML_CASE_NODE( "Node" )
			{
				String name;
				String type;

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

			XML_CASE_NODE( "Hide" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", m_hide);
				}
			}
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Color", "Value", &Node::setLocalColor );
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
		if( m_moveTo.isStarted() )
		{
			mt::vec2f pos;
			bool end = m_moveTo.update( _timing, &pos );
			setLocalPosition( pos );
			if( end == true )
			{
				callEvent( EVENT_MOVE_END, "(O)", getEmbedding() );
			}
		}
		if( m_colorLocalTo.isStarted() )
		{
			ColourValue newColor;
			m_colorLocalTo.update( _timing, &newColor );
			setLocalColor( newColor );
			if( m_colorLocalTo.isStarted() == false )
			{
				this->callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::updatable() const
	{
		return m_updatable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		bool done = true;

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
			/*MENGE_LOG( MENGE_TEXT("Error: compiled Node '%s' is failed\n")
				, getName().c_str() 
				);*/

			return false;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
	void Node::render( unsigned int _debugMask )
	{
		if( isRenderable() == false )
		{
			return;
		}

		if( checkVisibility() == false )
		{
			return;
		}

		_render( _debugMask );

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->render( _debugMask );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_render( unsigned int _debugMask )
	{
#	ifndef MENGE_MASTER_RELEASE
		if( _debugMask & MENGE_DEBUG_NODES )
		{
			const mt::box2f& bbox = getBoundingBox();
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			mt::vec2f size = box_size( bbox );
			renderEngine->renderLine( 0xFF00FF00, bbox.min, bbox.min + mt::vec2f( size.x, 0.0f ) );
			renderEngine->renderLine( 0xFF00FF00, bbox.min, bbox.min + mt::vec2f( 0.0f, size.y ) );
			renderEngine->renderLine( 0xFF00FF00, bbox.max, bbox.max - mt::vec2f( size.x, 0.0f ) );
			renderEngine->renderLine( 0xFF00FF00, bbox.max, bbox.max - mt::vec2f( 0.0f, size.y ) );
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::checkVisibility()
	{
		if( m_changeVisibility )
		{
			const Viewport& viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
			m_visibility = _checkVisibility( viewport );
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
		const mt::box2f & bbox = getBoundingBox();

		bool result = _viewport.testBBox( bbox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = Holder<ScriptEngine>::hostage()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateWorldMatrix()
	{
		Allocator2D::invalidateWorldMatrix();

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->invalidateWorldMatrix();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		Allocator2D::_invalidateWorldMatrix();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Node::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return Allocator2D::getLocalMatrix();
		}

		if( isInvalidateWorldMatrix() == false )
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

		//!!!!!
		static mt::vec2f v;
		v = mt::vec2f( wm.m[6], wm.m[7] );
		return v;
		//return wm.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		//!!!!!
		static mt::vec2f v;
		v = mt::vec2f( wm.m[0], wm.m[1] );
		return v;
		//return wm.v0.v2;
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
		registerEvent( EVENT_MOVE_STOP, ("onMoveStop"), m_listener );
		registerEvent( EVENT_MOVE_END, ("onMoveEnd"), m_listener );
		registerEvent( EVENT_COLOR_END, ("onColorEnd"), m_listener );
		registerEvent( EVENT_COLOR_STOP, ("onColorStop"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer2D * _layer )
	{
		m_layer = _layer;

		// ïðîñòè ãîñïîäè
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
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
		mt::vec2f screen_pos = pos;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		if( m_layer )
		{
			screen_pos = m_layer->calcScreenPosition( viewport, this );
		}

		return screen_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::colorTo( const ColourValue & _color, float _time )
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			(*it)->colorTo( _color, _time );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setAlpha( float _alpha ) 
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			(*it)->setAlpha( _alpha );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::alphaTo( float _alpha, float _time ) 
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			(*it)->alphaTo( _alpha, _time );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::colorToStop()
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			(*it)->colorToStop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveTo( float _time, const mt::vec2f& _point )
	{
		const mt::vec2f& pos = getWorldPosition();

		if( m_moveTo.start( pos, _point, _time, mt::length_v2 ) == false )
		{
			setLocalPosition( _point );
			callEvent( EVENT_MOVE_END, "(O)", getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveToCb( float _time, const mt::vec2f& _point, PyObject* _cb )
	{
		const mt::vec2f& pos = getWorldPosition();

		if( m_moveTo.start( pos, _point, _time, mt::length_v2 ) == false )
		{
		   setLocalPosition( _point );
		   callEvent( EVENT_MOVE_END, "(O)", getEmbedding() );
		   pybind::call( _cb, "(O)", getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveToStop()
	{
		m_moveTo.stop();
		callEvent( EVENT_MOVE_STOP, "(O)", getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveToStopCb(PyObject* _cb)
	{
		m_moveTo.stop();
		callEvent( EVENT_MOVE_STOP, "(O)", getEmbedding() );
		pybind::call( _cb, "(O)", getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & Node::getBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			_updateBoundingBox( m_boundingBox );

			for( TContainerChildren::iterator
				it = m_children.begin(),
				it_end = m_children.end();
			it != it_end;
			++it )
			{
				if( (*it)->isRenderable() == false )
				{
					continue;
				}
				const mt::box2f & childrenBoundingBox = (*it)->getBoundingBox();

				mt::merge_box( m_boundingBox, childrenBoundingBox );
			}

			m_invalidateBoundingBox = false;
		}

		return m_boundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateBoundingBox()
	{
		m_invalidateBoundingBox = true;

		if( m_parent )
		{
			m_parent->invalidateBoundingBox();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		const mt::vec2f & wp = this->getWorldPosition();

		mt::reset( _boundingBox, wp );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::getUpdatable()
	{
		if( ( isUpdatable() /*&& m_updatable*/ ) == false )
		{
			return false;
		}
		if( m_parent )
		{
			return m_parent->getUpdatable();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLocalColor( const ColourValue& _color )
	{
		m_colorLocal = _color;
		invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateColor()
	{
		m_invalidateColor = true;
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end()
			; it != it_end
			; it++ )
		{
			(*it)->invalidateColor();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& Node::getWorldColor()
	{
		if( m_invalidateColor == true )
		{
			m_colorWorld = m_colorLocal;
			if( m_parent != NULL )
			{
				m_colorWorld = m_colorWorld * m_parent->getWorldColor();
			}
			m_invalidateColor = false;
		}
		return m_colorWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& Node::getLocalColor() const
	{
		return m_colorLocal;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLocalAlpha( float _alpha )
	{
		m_colorLocal.setA( _alpha );
		setLocalColor( m_colorLocal );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorTo( float _time, const ColourValue& _color )
	{
		if( m_colorLocalTo.isStarted() )
		{
			this->callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
			m_colorLocalTo.stop();
		}
		if( m_colorLocalTo.start( m_colorLocal, _color, _time, length_color ) == false )
		{
			setLocalColor( _color );
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorToCb( float _time, const ColourValue& _color, PyObject* _cbStop, PyObject* _cbEnd)
	{
		if( m_colorLocalTo.isStarted() )
		{
			this->callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
			pybind::call( _cbStop, "(O)", getEmbedding() );
			m_colorLocalTo.stop();
		}
		if( m_colorLocalTo.start( m_colorLocal, _color, _time, length_color ) == false )
		{
			setLocalColor( _color );
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
			pybind::call( _cbEnd, "(O)", getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localAlphaTo( float _time, float _alpha )
	{
		ColourValue color = m_colorLocal;
		color.setA( _alpha );
		localColorTo( _time, color );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorToStop()
	{
		m_colorLocalTo.stop();
		callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorToStopCb(PyObject* _cb)
	{
		m_colorLocalTo.stop();
		callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
		pybind::call( _cb, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
}