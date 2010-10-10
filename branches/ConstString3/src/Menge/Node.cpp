#	include "Node.h"
#	include "Visitor.h"

#	include "Layer2D.h"

#	include "NodeManager.h"
#	include "EntityManager.h"

#	include "Entity.h"

#	include "RenderEngine.h"
#	include "ScriptEngine.h"
#	include "Logger/Logger.h"

#	include "BinParser.h"

#	include "Player.h"
#	include "Camera2D.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

#	include "Material.h"
#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "Application.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_active(false)
		, m_enable(true)
		, m_updatable(true)
		, m_state(NODE_IDLE)
		, m_parent(0)
		, m_layer(0)
		, m_cameraRevision(0)
#	ifndef MENGE_MASTER_RELEASE
		, m_debugMaterial( NULL )
#	endif
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node::~Node()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
		this->release();

		if( m_parent )
		{
			m_parent->removeChildren( this );
		}

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;)
		{
			(*it)->setParent(0);

			TContainerChildren::iterator it_next = m_children.erase( it );
			(*it)->destroy();
			it = it_next;
		}

#ifndef MENGE_MASTER_RELEASE
		RenderEngine::get()
			->releaseMaterial( m_debugMaterial );
#endif // MENGE_MASTER_RELEASE
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
		if( isCompile() == false )
		{
			if( compile() == false )
			{
				/*MENGE_LOG_INFO( MENGE_TEXT("Error: activation of Node '%s' is failed, because compilation is failed\n")
					, m_name.c_str() 
					);*/

				return false;
			}
		}

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

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		if( m_state == NODE_UPDATING )
		{
			m_state = NODE_DEACTIVATING;
			return;
		}

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

		m_cameraRevision = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::enable()
	{
		m_enable = true;
		
		this->_enable();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::disable()
	{
		m_enable = false;

		this->_disable();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_enable()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_disable()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setParent( Node * _parent )
	{
		m_parent = _parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren( Node * _node )
	{
		return addChildren_( _node, m_children.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenFront( Node* _node )
	{
		return addChildren_( _node, m_children.begin() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren_( Node * _node, TContainerChildren::iterator _insert )
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
			m_children.erase( _node );
			m_children.insert( _insert, _node );
		}
		else
		{
			if( parent )
			{
				parent->removeChildren( _node );
			}

			_node->setParent( this );
			_node->setLayer( m_layer );

			m_children.insert( _insert, _node );

			if( parent )
			{
				_node->_changeParent( parent );
			}
		}

		_node->invalidateWorldMatrix();

		_addChildren( _node );

		invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
		this->_removeChildren( _node );

		TContainerChildren::iterator it_find = 
			intrusive_find( m_children.begin(), m_children.end(), _node );

		if( it_find != m_children.end() )
		{
			(*it_find)->setParent( 0 );
			m_children.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FFindChildByName
		{
		public:
			FFindChildByName( const ConstString & _name )
				: m_name(_name)
			{
			}

		public:
			bool operator () ( Node * _node ) const
			{
				return _node->getName() == m_name;
			}

		protected:
			const ConstString & m_name;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::getChildren( const ConstString & _name, bool _recursion ) const
	{
		TContainerChildren::const_iterator it_found =
			intrusive_find_if( m_children.begin(), m_children.end(), FFindChildByName(_name) );

		if( it_found != m_children.end() )
		{
			return *it_found;
		}

		if( _recursion == false )
		{
			return 0;
		}

		for( TContainerChildren::const_iterator 
			it = m_children.begin(), 
			it_end = m_children.end();
		it != it_end;
		it++ )
		{
			if( Node * node = (*it)->getChildren( _name, true ) )
			{
				return node;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isChildren( Node * _node, bool _recursive ) const
	{
		TContainerChildren::const_iterator it_find = 
			intrusive_find( m_children.begin(), m_children.end(), _node );

		if( it_find != m_children.end() )
		{
			return true;
		}

		if( _recursive == false )
		{
			return false;
		}
		
		for( TContainerChildren::const_iterator 
			it = m_children.begin(), 
			it_end = m_children.end();
		it != it_end;
		it++ )
		{
			if( (*it)->isChildren( _node, _recursive ) == true )
			{
				return true;
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changeParent( Node * _parent )
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
	bool Node::registerSelfEvent( EEventName _name, const char * _method )
	{
		PyObject * obj = this->getEmbed();
		pybind::decref( obj );

		bool result = Eventable::registerEvent( _name, _method, obj );

		return result;
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
	void Node::update( float _timing )
	{
		if( this->isUpdatable() == false )
		{
			return;
		}

		if( m_updatable )	// !!!!
		{
			m_state = NODE_UPDATING;
			_update( _timing );

			Affectorable::update( _timing );
			
			if( m_state == NODE_DEACTIVATING )
			{
				this->deactivate();				
			}

			m_state = NODE_IDLE;
		}

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		/*++it*/)
		{
			(*it++)->update( _timing );
		}

		if( m_updatable )	// !!!!
		{
			this->_postUpdate( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::loader( BinParser * _parser )
	{
		Transformation2D::loader( _parser );
		Renderable::loader( _parser );
		Colorable::loader( _parser );

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::Node )
			{
				ConstString name;
				ConstString type;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Node_Name, name );
					BIN_CASE_ATTRIBUTE( Protocol::Node_Type, type );
				}

				Node * node = NodeManager::get()
					->createNode( name, type );

				if(node == 0)
				{
					continue;
				}

				this->addChildren( node );

				BIN_PARSE_METHOD( node, &Node::loader );
			}

			BIN_CASE_NODE( Protocol::Entity )
			{
				ConstString name;
				ConstString prototype;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Entity_Name, name );
					BIN_CASE_ATTRIBUTE( Protocol::Entity_Type, prototype );
				}

				Entity * en = EntityManager::get()
					->createEntity( name, prototype );

				if( en == 0 )
				{
					continue;
				}

				this->addChildren( en );

				BIN_PARSE_METHOD( en, &Entity::loader );
			}

			BIN_CASE_ATTRIBUTE_METHOD_IF( Protocol::Enable_Value, &Node::enable, &Node::disable );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::loaded()
	{
		this->_loaded();

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it )
		{
			(*it)->_loaded();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
#ifndef MENGE_MASTER_RELEASE
		m_debugMaterial = RenderEngine::get()
							->createMaterial();

		//m_debugMaterial->textureStages = 1;
		m_debugMaterial->isSolidColor = false;
		m_debugMaterial->blendSrc = BF_SOURCE_ALPHA;
		m_debugMaterial->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

		m_debugMaterial->textureStage[0].colorOp = TOP_SELECTARG2;
		m_debugMaterial->textureStage[0].alphaOp = TOP_SELECTARG2;

		ApplyColor2D applyColor( 0xFF00FF00 );

		std::for_each( m_debugBox, m_debugBox + 4, applyColor );
#endif // MENGE_MASTER_RELEASE

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_deactivate()
	{
		Affectorable::clear();

#ifndef MENGE_MASTER_RELEASE
		RenderEngine::get()
			->releaseMaterial( m_debugMaterial );

		m_debugMaterial = 0;
#endif // MENGE_MASTER_RELEASE
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_update( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_postUpdate( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		//bool done = true;

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->compile();
		}

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
	bool Node::isRenderable() const
	{
		if( isCompile() == false )
		{
			return false;
		}

		if( isEnable() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( isHide() == true )
		{
			return false;
		}

		const ColourValue & colour = getWorldColor();

		if( colour.getA() < 0.001f )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::render( Camera2D * _camera )
	{
		if( isRenderable() == true )
		{
			const Viewport& viewPort = _camera->getViewport();

			//std::size_t cameraRevision = _camera->getCameraRevision();

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

			if( checkVisibility( viewPort ) == true )
			{
				_render( _camera );

				renderChild( _camera );
			}
			//}
		}


#	ifndef	MENGE_MASTER_RELEASE

		if( isCompile() == true && isActivate() == true )
		{
			unsigned int debugMask = Application::get()->getDebugMask();
			_debugRender( _camera, debugMask );
		}

#	endif	// MENGE_MASTER_RELEASE

	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild( Camera2D * _camera )
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->render( _camera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
		const mt::box2f & bbox = this->getBoundingBox();

		bool result = _viewport.testBBox( bbox );

		return result;
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
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->invalidateWorldMatrix();
		}

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Node::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return Transformation2D::getLocalMatrix();
		}

		if( Transformation2D::isInvalidateWorldMatrix() == false )
		{
			return Transformation2D::getRelationMatrix();
		}

		const mt::mat3f & wm = m_parent->getWorldMatrix();

		return Transformation2D::updateWorldMatrix( wm );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldPosition()
	{
		const mt::mat3f &wm = this->getWorldMatrix();

		return wm.v2.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldDirection()
	{
		const mt::mat3f &wm = this->getWorldMatrix();

		return wm.v0.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setListener( PyObject * _listener )
	{
		this->_setListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setListener( PyObject * _listener )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer * _layer )
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
	Layer * Node::getLayer() const
	{
		return m_layer;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Node::getScene() const
	{
		if( m_layer == 0 )
		{
			return 0;
		}

		return m_layer->getScene();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Node::getScreenPosition()
	{
		const mt::vec2f & pos = getWorldPosition();
		mt::vec2f screen_pos = pos;

		Camera2D * camera = Player::get()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		if( m_layer )
		{
			screen_pos = m_layer->calcScreenPosition( viewport, this );
		}

		return screen_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateBoundingBox()
	{
		invalidateVisibility();

		if( m_parent )
		{
			m_parent->invalidateBoundingBox();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			m_invalidateBoundingBox = false;

			_updateBoundingBox( m_boundingBox );

			for( TContainerChildren::iterator
				it = m_children.begin(),
				it_end = m_children.end();
			it != it_end;
			++it )
			{
				if( (*it)->isCompile() == false )
				{
					continue;
				}

				const mt::box2f & childrenBoundingBox = (*it)->getBoundingBox();

				mt::merge_box( m_boundingBox, childrenBoundingBox );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		const mt::vec2f & wp = this->getWorldPosition();

		mt::reset( _boundingBox, wp );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::getUpdatable() const
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
	void Node::_invalidateColor()
	{
		for( TContainerChildren::iterator 
			it = m_children.begin(), 
			it_end = m_children.end()
			; it != it_end
			; it++ )
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
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Node::_debugRender( Camera2D* _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_NODES )
		{
			const mt::box2f& bbox = getBoundingBox();
			RenderEngine* renderEngine = RenderEngine::get();
			//mt::vec2f size = box_size( bbox );
			m_debugBox[0].pos[0] = bbox.minimum.x;
			m_debugBox[0].pos[1] = bbox.minimum.y;
			m_debugBox[1].pos[0] = bbox.maximum.x;
			m_debugBox[1].pos[1] = bbox.minimum.y;
			m_debugBox[2].pos[0] = bbox.maximum.x;
			m_debugBox[2].pos[1] = bbox.maximum.y;
			m_debugBox[3].pos[0] = bbox.minimum.x;
			m_debugBox[3].pos[1] = bbox.maximum.y;
			renderEngine->renderObject2D( m_debugMaterial, NULL, 1, m_debugBox, 4, LPT_RECTANGLE );
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
}
