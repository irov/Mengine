#	include "Node.h"
#	include "Visitor.h"

#	include "Layer2D.h"
#	include "Scene.h"

#	include "NodeManager.h"
#	include "EntityManager.h"

#	include "Entity.h"

#	include "RenderEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include "Player.h"
#	include "Camera2D.h"

#	include "Consts.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

#	include "RenderMaterial.h"
#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "Application.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_active(false)
		, m_enable(true)
		, m_freeze(false)
		, m_parent(0)
		, m_layer(0)
		, m_cameraRevision(0)
#	ifndef MENGE_MASTER_RELEASE
		, m_debugMaterial(NULL)
#	endif
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node::~Node()
	{
		this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
		this->release();

		this->removeFromParent();

		this->destroyAllChild();
	}	
	//////////////////////////////////////////////////////////////////////////
	void Node::visit( Visitor * _visitor )
	{
		_visitor->visit_impl( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->visit( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::activate()
	{
		if( this->isEnable() == false )
		{
			return true;
		}

		if( this->isCompile() == false )
		{
			if( this->compile() == false )
			{
				//MENGE_LOG_INFO( "Error: activation of Node '%s' is failed, because compilation is failed\n"
				//	, m_name.c_str() 
				//	);

				return false;
			}
		}

		if( m_active == true )
		{
			return true;
		}

		if( this->_activate() == false )
		{
			return false;
		}

		m_active = true;

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();	
		it != it_end;
		++it)
		{
			(*it)->activate();
			//if( (*it)->activate() == false )
			//{
			//	return false;
			//}
		}
	
		this->_afterActivate();

		//if( m_active == false )
		//{
		//	MENGE_LOG_INFO( "Error: activation of Node '%s' is failed\n"
		//		, m_name.c_str() 
		//		);
		//}

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		if( isEnable() == false )
		{
			return;
		}

		if( m_active == false )
		{
			return;
		}

		m_cameraRevision = 0;

		this->_deactivate();		

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->deactivate();
		}	

		m_active = false;

		this->_afterDeactivate();				
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::enable()
	{
		if( this->isActivate() == true )
		{
			return true;
		}

		m_enable = true;

		if( m_parent )
		{
			if( m_parent->isActivate() == false )
			{
				return true;
			}
		}

		if( this->activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::disable()
	{
		if( this->isActivate() == true )
		{
			this->deactivate();
		}

		m_enable = false;

		//this->_disable();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setParent_( Node * _parent )
	{
		Node * oldparent = m_parent;
		m_parent = _parent;

		this->_changeParent( oldparent, _parent );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::destroyAllChild()
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;)
		{
			Node * node = (*it);
			node->setParent_(0);

			TListChild::iterator it_next = m_child.erase( it );
			node->destroy();
			it = it_next;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeAllChild()
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it )
		{
			(*it)->setParent_( 0 );
			(*it)->setLayer( 0 );
		}

		m_child.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeFromParent()
	{
		if( m_parent == 0 )
		{
			return;
		}

		m_parent->removeChildren( this );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren( Node * _node )
	{
		return this->addChildren_( _node, m_child.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenFront( Node* _node )
	{
		return this->addChildren_( _node, m_child.begin() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenAfter( Node* _node, Node * _after )
	{
		TListChild::iterator it_found = 
			intrusive_find( m_child.begin(), m_child.end(), _after );

		if( it_found == m_child.end() )
		{
			return false;
		}

		return this->addChildren_( _node, it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren_( Node * _node, TListChild::iterator _insert )
	{
		/*if( this->isChildren( _node, false ) )
		{
			//MENGE_LOG_ERROR( "Node '%s' type '%s' addChildren failed '%s' because type '%s' is already exist"
			//, this->getName().c_str()
			//, this->getType().c_str()
			//, _node->getName().c_str()
			//, _node->getType().c_str() );
			return false;
		}*/
		Node * parent = _node->getParent();

		if( parent == this )
		{
			m_child.erase( _node );
			m_child.insert( _insert, _node );
		}
		else
		{
			if( parent )
			{
				parent->removeChildren( _node );
			}

			m_child.insert( _insert, _node );

			_node->setParent_( this );
			_node->setLayer( m_layer );
		}

		if( this->isActivate() == false && _node->isActivate() == true )
		{
			_node->deactivate();
		}
		else if( this->isActivate() == true && _node->isActivate() == false && _node->isEnable() == true )
		{
			_node->activate();
		}

		_node->invalidateWorldMatrix();

		this->_addChildren( _node );

		this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TListChild & Node::getChild() const
	{
		return m_child;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
		this->_removeChildren( _node );

		TListChild::iterator it_find = 
			intrusive_find( m_child.begin(), m_child.end(), _node );

		if( it_find != m_child.end() )
		{
			(*it_find)->setParent_( 0 );
			(*it_find)->setLayer( 0 );

			m_child.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Impl
	{
		template<class M>
		class FFindChild
		{
		public:
			FFindChild( M _method, const ConstString & _name )
				: m_method(_method)
				, m_name(_name)
			{
			}

		public:
			bool operator () ( Node * _node ) const
			{
				return (_node->*m_method)() == m_name;
			}

		protected:
			M m_method;
			const ConstString & m_name;
		};

		template<class C, class M>
		typename C::const_iterator find_child( const C & _child, M _method, const ConstString & _name )
		{
			typename C::const_iterator it_found =
				intrusive_find_if( _child.begin(), _child.end(), FFindChild<M>( _method, _name) );

			return it_found;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::findChildren( const ConstString & _name, bool _recursion ) const
	{
		TListChild::const_iterator it_found =
			Impl::find_child( m_child, &Identity::getName, _name );

		if( it_found != m_child.end() )
		{
			return *it_found;
		}

		if( _recursion )
		{
			for( TListChild::const_iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
			{
				if( Node * node = (*it)->findChildren( _name, true ) )
				{
					return node;
				}
			}
		}

		if( Node * node = this->_findChildren( _name, _recursion ) )
		{
			return node;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::findTag( const ConstString & _tag ) const
	{
		TListChild::const_iterator it_found =
			Impl::find_child( m_child, &Identity::getTag, _tag );

		if( it_found != m_child.end() )
		{
			return *it_found;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::_findChildren( const ConstString & _name, bool _recursion ) const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::hasChildren( const ConstString & _name, bool _recursive ) const
	{
		TListChild::const_iterator it_found =
			Impl::find_child( m_child, &Identity::getName, _name );

		if( it_found != m_child.end() )
		{
			return true;
		}

		if( _recursive )
		{		
			for( TListChild::const_iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
			{
				if( (*it)->hasChildren( _name, true ) == true )
				{
					return true;
				}
			}
		}

		if( this->_hasChildren( _name, _recursive ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::emptyChild() const
	{
		return m_child.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_hasChildren( const ConstString & _name, bool _recursive ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changeParent( Node * _oldParent, Node * _newParent )
	{
		//Empty
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
	void Node::freeze( bool _value )
	{
		if( this->isFreeze() == _value )
		{
			return;
		}

		m_freeze = _value;

		this->_freeze( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_freeze( bool _value )
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			Node * node = *it;

			if( node->isFreeze() != _value )
			{
				(*it)->_freeze( _value );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::update( float _timing )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		if( this->isFreeze() == true )
		{
			return;
		}

		_update( _timing );

		Affectorable::update( _timing );
		
		//size_t childCount = m_child.size();

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			it->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
		GlobalHandleAdapter::activateGlobalHandle();

#ifndef MENGE_MASTER_RELEASE
		const RenderMaterialGroup * mg_debug = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(Debug) );

		m_debugMaterial = mg_debug->getMaterial( TAM_CLAMP, TAM_CLAMP );

		ApplyColor2D applyColor( 0xFF00FF00 );

		std::for_each( m_debugBox, m_debugBox + 4, applyColor );
#endif // MENGE_MASTER_RELEASE

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_afterActivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_deactivate()
	{
		Affectorable::stopAllAffectors();
		//Affectorable::clear();

		GlobalHandleAdapter::deactivateGlobalHandle();

#ifndef MENGE_MASTER_RELEASE
		m_debugMaterial = 0;
#endif // MENGE_MASTER_RELEASE
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_afterDeactivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_update( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		//bool done = true;

		//for( TListChild::iterator
		//	it = m_child.begin(),
		//	it_end = m_child.end();
		//it != it_end;
		//++it)
		//{
		//	(*it)->compile();
		//}

		//if( done == false )
		//{
		//	return false;
		//}

		bool result = Resource::compile();

		if( result == false )
		{
			/*MENGE_LOG_INFO( MENGE_TEXT("Error: compiled Node '%s' is failed\n")
				, getName().c_str() 
				);*/

			return false;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		deactivate();

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->release();
		}

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_recompile()
	{
		if( m_enable == false )
		{
			return;
		}

		if( m_active == true )
		{
			return;
		}

		this->activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::render( RenderCameraInterface * _camera )
	{
		if( this->isRenderable() == true )
		{
			//const Viewport& viewPort = _camera->getViewport();

			//size_t cameraRevision = _camera->getCameraRevision();

			//if( m_cameraRevision == cameraRevision && this->isInvalidateVisibility() == false )
			//{
			//	if( getVisibility() == false )
			//	{
			//		return;
			//	}
			//}
			//else
			//{
			//	m_cameraRevision = cameraRevision;

			//if( this->checkVisibility( viewPort ) == true )
			{
				if( this->isLocalHide() == false && this->isFullBlend() == false )
				{
					_render( _camera );
				}

				renderChild( _camera );
			}
			//}
		}


#	ifndef	MENGE_MASTER_RELEASE

		if( this->isActivate() == true )
		{
			unsigned int debugMask = Application::get()
				->getDebugMask();

			_debugRender( _camera, debugMask );
		}

#	endif	// MENGE_MASTER_RELEASE

	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild( RenderCameraInterface * _camera )
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->render( _camera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
		//const mt::box2f & bbox = this->getBoundingBox();

		//bool result = _viewport.testBBox( bbox );

		//return result;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = ScriptEngine::get()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->invalidateWorldMatrix();
		}

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Node::getWorldMatrix() const
	{
		if( m_parent == 0 )
		{
			return Transformation3D::getLocalMatrix();
		}

		if( Transformation3D::isInvalidateWorldMatrix() == false )
		{
			return Transformation3D::getRelationMatrix();
		}

		const mt::mat4f & wm = m_parent->getWorldMatrix();

		const mt::mat4f & update_wm = Transformation3D::updateWorldMatrix( wm );

		return update_wm;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Node::getWorldPosition()
	{
		const mt::mat4f &wm = this->getWorldMatrix();

		return wm.v3.to_vec3f();
	}
	////////////////////////////////////////////////////////////////////////////
	//const mt::vec2f & Node::getWorldDirection()
	//{
	//	const mt::mat3f &wm = this->getWorldMatrix();

	//	return wm.v0.to_vec2f();
	//}
	//////////////////////////////////////////////////////////////////////////
	void Node::setWorldPosition( const mt::vec3f & _pos )
	{
		const mt::vec3f & wp = this->getWorldPosition();

		mt::vec3f wp_offset = _pos - wp;

		this->translate( wp_offset );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::setEventListener( PyObject * _args, PyObject * _kwds )
	{
		if( _kwds == 0 )
		{
			MENGE_LOG_ERROR("Node %s setEventListener wait kwds"
				, m_name.c_str() 
				);

			return pybind::ret_none();
		}
		
		this->_setEventListener( _kwds );
		
		return pybind::ret_none();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeEventListener()
	{
		this->_setEventListener( 0 );		
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setEventListener( PyObject * _listener )
	{
		GlobalHandleAdapter::setEventListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_embedding( PyObject * _embed )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isEnableGlobalHandle() const
	{
		return this->isActivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer * _layer )
	{
		m_layer = _layer;

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			(*it)->setLayer( _layer );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Node::getLayer() const
	{
		return m_layer;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Node::getScene()
	{
		if( m_layer == 0 )
		{
			return 0;
		}

		return m_layer->getScene();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Node::getCameraPosition( RenderCameraInterface * _camera )
	{
		const mt::vec3f & pos = this->getWorldPosition();
		mt::vec2f screen_pos = pos.to_vec2f();
		
		if( m_layer )
		{
			const Viewport & viewport = _camera->getViewport();

			m_layer->calcScreenPosition( screen_pos, viewport, this );
		}

		return screen_pos;
	}
	////////////////////////////////////////////////////////////////////////////
	//void Node::_invalidateBoundingBox()
	//{
	//	invalidateVisibility();

	//	if( m_parent )
	//	{
	//		m_parent->invalidateBoundingBox();
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	void Node::getBoundingBox( mt::box2f & _boundingBox )
	{
		const mt::box2f & _localBoundingBox = this->getLocalBoundingBox();
		
		_boundingBox = _localBoundingBox;
		//BoundingBox::updateBoundingBox();
		//m_invalidateBoundingBox = false;

		//this->_updateBoundingBox( m_boundingBox );
			
		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it )
		{
			Node * node = *it;
			
			if( node->isCompile() == false )
			{
				continue;
			}

			if( node->isEnable() == false )
			{
				continue;
			}

			mt::box2f childrenBoundingBox;
			node->getBoundingBox( childrenBoundingBox );

			mt::merge_box( _boundingBox, childrenBoundingBox );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		const mt::vec3f & wp = this->getWorldPosition();

		const mt::vec2f & v2 = wp.to_vec2f();
		mt::reset( _boundingBox, v2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateColor()
	{
		ColourValue colour;
		this->calcTotalColor(colour);

		bool fullBlend = (colour.getA() < 0.001f);

		this->setFullBlend( fullBlend );

		for( TListChild::iterator 
			it = m_child.begin(), 
			it_end = m_child.end();
		it != it_end;
		++it )
		{
			(*it)->invalidateColor();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Node::getWorldColor() const
	{
		if( m_parent == 0 )
		{
			return Colorable::getLocalColor();
		}

		if( Colorable::isInvalidateColor() == false )
		{
			return Colorable::getRelationColor();
		}

		const ColourValue & parentColor = m_parent->getWorldColor();

		return Colorable::updateRelationColor( parentColor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::calcTotalColor( ColourValue & _color ) const
	{
		const ColourValue & worldColour = this->getWorldColor();
		_color = worldColour;

		const ColourValue & personalColour = this->getPersonalColor();		
		_color *= personalColour;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitResource( VisitorResource * _visitor )
	{
		//Empty
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Node::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_NODES )
		{
			mt::box2f bbox;
			this->getBoundingBox( bbox );
			
			m_debugBox[0].pos[0] = bbox.minimum.x;
			m_debugBox[0].pos[1] = bbox.minimum.y;
			m_debugBox[0].pos[2] = 0.f;
			//m_debugBox[0].pos[3] = 1.f;

			m_debugBox[1].pos[0] = bbox.maximum.x;
			m_debugBox[1].pos[1] = bbox.minimum.y;
			m_debugBox[1].pos[2] = 0.f;
			//m_debugBox[1].pos[3] = 1.f;

			m_debugBox[2].pos[0] = bbox.maximum.x;
			m_debugBox[2].pos[1] = bbox.maximum.y;
			m_debugBox[2].pos[2] = 0.f;
			//m_debugBox[2].pos[3] = 1.f;

			m_debugBox[3].pos[0] = bbox.minimum.x;
			m_debugBox[3].pos[1] = bbox.maximum.y;
			m_debugBox[3].pos[2] = 0.f;
			//m_debugBox[3].pos[3] = 1.f;

			m_debugBox[4].pos[0] = bbox.minimum.x;
			m_debugBox[4].pos[1] = bbox.minimum.y;
			m_debugBox[4].pos[2] = 0.f;
			//m_debugBox[4].pos[3] = 1.f;

			RenderEngine::get()
				->renderObject2D( m_debugMaterial, NULL, NULL, 0, m_debugBox, 5, LPT_LINE );
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
}
