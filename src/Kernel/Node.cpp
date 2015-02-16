#	include "Kernel/Node.h"

#   include "Kernel/Layer.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/NodeInterface.h"

#	include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_serviceProvider(nullptr)
        , m_active(false)
		, m_deactivating(false)
		, m_afterActive(false)
		, m_enable(true)
		, m_freeze(false)
		, m_speedFactor(1.f)
		, m_rendering(false)
		, m_parent(nullptr)
		, m_layer(nullptr)
		, m_cameraRevision(0)
		, m_renderCamera(nullptr)
		, m_renderViewport(nullptr)
		, m_shallowGrave(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node::~Node()
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Node::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setShallowGrave()
    {
        ++m_shallowGrave;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeShallowGrave()
    {
        --m_shallowGrave;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool Node::isShallowGrave() const
    {
        return m_shallowGrave > 0;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::destroy()
	{
		if( this->isShallowGrave() == true )
		{
			NODE_SERVICE(m_serviceProvider)
				->addHomeless( this );

			this->release();

			return;
		}

		Factorable::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
        this->release();

        this->destroyAllChild();
		this->removeFromParent();

        this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::activate()
	{
		if( this->isEnable() == false )
		{
			return true;
		}

        if( this->compile() == false )
        {
            //MENGE_LOG_INFO( "Error: activation of Node '%s' is failed, because compilation is failed\n"
            //	, m_name.c_str() 
            //	);

            return false;
        }

		if( m_active == true )
		{
			return true;
		}

		if( this->_activate() == false )
		{
			return false;
		}

        this->setShallowGrave();

		m_active = true;

        this->updateRendering_();

		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->activate();
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * children = (*it);

				children->activate();
			}
		}
	
		m_afterActive = true;

		this->_afterActivate();

		//if( m_active == false )
		//{
		//	MENGE_LOG_INFO( "Error: activation of Node '%s' is failed\n"
		//		, m_name.c_str() 
		//		);
		//}
        
		this->removeShallowGrave();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		if( m_active == false )
		{
			return;
		}

		if( m_deactivating == true )
		{
			return;
		}

		m_deactivating = true;

		if( m_afterActive == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::deactivate %s invalid deactivate in 'activate state'"
				, this->getName().c_str()
				);

			//return;
		}

		m_afterActive = false;

        this->setShallowGrave();

		m_cameraRevision = 0;

		this->_deactivate();

		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->deactivate();
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * children = *it;

				children->deactivate();
			}
		}

		m_active = false;
		m_deactivating = false;
				
		this->_afterDeactivate();		

		this->updateRendering_();

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::enable()
	{
		if( this->isActivate() == true )
		{
			return true;
		}

		m_enable = true;

		if( m_parent != nullptr )
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
        this->setShallowGrave();

        this->deactivate();

        //this->release();

		m_enable = false;

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setParent_( Node * _parent )
	{
		Node * oldparent = m_parent;
		m_parent = _parent;

		this->setRelationTransformation( _parent );

		this->_changeParent( oldparent, _parent );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::destroyAllChild()
	{
		for( TSlugChild it(m_child); it.eof() == false; )
		{
			Node * node = (*it);

			node->setParent_( nullptr );

			TListNodeChild::iterator it_node(node);

			it.next_shuffle();

			m_child.erase( it_node );

            node->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren()
	{
		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
            Node * node = (*it);

            //NODE_SERVICE(m_serviceProvider)
            //    ->addHomeless( node );
            node->deactivate();

            Node * parent = node->getParent();

            if( parent != this )
            {
                continue;
            }

            node->setParent_( nullptr );
            node->setLayer( nullptr );
		}

		m_child.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::removeFromParent()
	{
		if( m_parent == nullptr )
		{
			return true;
		}

		bool result = m_parent->removeChild( this );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChild( Node * _node )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::addChildren '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return false;
		}

		bool result = this->addChild_( m_child.end(), _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildFront( Node * _node )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::addChildrenFront '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return false;
		}

		bool result = this->addChild_( m_child.begin(), _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildAfter( Node* _node, Node * _after )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::addChildrenAfter '%s' invalid add NULL node (node)"
				, this->getName().c_str()
				);

			return false;
		}

		if( _after == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::addChildrenAfter '%s' invalid add NULL node (after)"
				, this->getName().c_str()
				);

			return false;
		}

		TListNodeChild::iterator it_found = 
			intrusive_find( m_child.begin(), m_child.end(), _after );

		if( it_found == m_child.end() )
		{
			return false;
		}

		bool result = this->addChild_( it_found, _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChild_( TListNodeChild::iterator _insert, Node * _node )
	{
		//if( this->isChildren( _node, false ) )
		//{
		//	//MENGE_LOG_ERROR( "Node '%s' type '%s' addChildren failed '%s' because type '%s' is already exist"
		//	//, this->getName().c_str()
		//	//, this->getType().c_str()
		//	//, _node->getName().c_str()
		//	//, _node->getType().c_str() );
		//	return false;
		//}

		Node * parent = _node->getParent();

        _node->setShallowGrave();

		if( parent == this )
		{
			if( *_insert == _node )
			{
				return true;
			}
			
			this->eraseChild_( _node );
			
			this->insertChild_( _insert, _node );			
		}
		else
		{
			if( parent != nullptr )
			{
				parent->removeChild_( _node );
			}

			this->insertChild_( _insert, _node );

            _node->setLayer( m_layer );
			_node->setParent_( this );
		}

		this->_addChild( _node );

		if( this->isActivate() == false && _node->isActivate() == true )
		{            
			_node->deactivate();
		}
		else if( this->isActivate() == true && _node->isActivate() == false )
		{
			_node->activate();
		}

		_node->invalidateWorldMatrix();
		_node->invalidateColor();

        _node->removeShallowGrave();

		//this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::insertChild_( TListNodeChild::iterator _insert, Node * _node )
	{
		m_child.insert( _insert, _node );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::eraseChild_( Node * _node )
	{
        TListNodeChild::iterator it_node(_node, true);
		m_child.erase( it_node );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		this->visit( _visitor );

		Node * single_child = m_child.single();

		if( single_child != nullptr )
		{
			single_child->visitChildren( _visitor );
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * children = (*it);

				children->visitChildren( _visitor );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::removeChild( Node * _node )
	{
#	ifdef _DEBUG
		if( stdex::intrusive_has( m_child.begin(), m_child.end(), _node ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Node::removeChild %s not found children %s"
                , this->getName().c_str()
                , _node->getName().c_str()
                );

            return false;
        }
#	endif

        _node->deactivate();

		//check if deactivate remove from parent!
		Node * parent = _node->getParent();

		if( parent != this )
		{
			return false;
		}

        this->removeChild_( _node );
		
		//NODE_SERVICE(m_serviceProvider)
		//	->addHomeless( _node );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::removeChild_( Node * _node )
    {
        this->_removeChild( _node );

        _node->setParent_( nullptr );
        _node->setLayer( nullptr );

        this->eraseChild_( _node );
    }
	//////////////////////////////////////////////////////////////////////////
	namespace
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

        private:
            void operator = ( const FFindChild & )
            {               
            }

		protected:
			M m_method;
			const ConstString & m_name;
		};

		template<class C, class M>
		static typename C::const_iterator s_node_find_child( const C & _child, M _method, const ConstString & _name )
		{
			typename C::const_iterator it_found =
				intrusive_find_if( _child.begin(), _child.end(), FFindChild<M>( _method, _name) );

			return it_found;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::findChild( const ConstString & _name, bool _recursion ) const
	{
		TListNodeChild::const_iterator it_found =
			s_node_find_child( m_child, &Identity::getName, _name );

		if( it_found != m_child.end() )
		{
			Node * children = *it_found;

			return children;
		}

		if( _recursion == true )
		{
			for( TListNodeChild::const_iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
			{
				Node * children = (*it);

				if( Node * node = children->findChild( _name, true ) )
				{
					return node;
				}
			}
		}

		if( Node * node = this->_findChild( _name, _recursion ) )
		{
			return node;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::_findChild( const ConstString & _name, bool _recursion ) const
	{
        (void)_name;
        (void)_recursion;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::hasChild( const ConstString & _name, bool _recursive ) const
	{
		TListNodeChild::const_iterator it_found =
			s_node_find_child( m_child, &Identity::getName, _name );

		if( it_found != m_child.end() )
		{
			return true;
		}

		if( _recursive == true )
		{		
			for( TListNodeChild::const_iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
			{
				Node * children = (*it);
				if( children->hasChild( _name, true ) == true )
				{
					return true;
				}
			}
		}

		if( this->_hasChild( _name, _recursive ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_hasChild( const ConstString & _name, bool _recursive ) const
	{
        (void)_name;
        (void)_recursive;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::emptyChildren() const
	{
		return m_child.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changeParent( Node * _oldParent, Node * _newParent )
	{
        (void)_oldParent;
        (void)_newParent;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_addChild( Node * _node )
	{
        (void)_node;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_removeChild( Node * _node )
	{
        (void)_node;
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
		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
			Node * node = *it;

			node->_freeze( _value );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setSpeedFactor( float _speedFactor )
	{
		m_speedFactor = _speedFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	float Node::getSpeedFactor() const
	{
		return m_speedFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::update( float _current, float _timing )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		if( this->isFreeze() == true )
		{
			return;
		}

		float total_timing = _timing * m_speedFactor;

		this->setShallowGrave();

		this->_update( _current, total_timing );

		Affectorable::updateAffectors( _current, total_timing );
		
		this->updateChild_( _current, total_timing );

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateChild_( float _current, float _timing )
	{
		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->update( _current, _timing );
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * children = *it;
			
				children->update( _current, _timing );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
		if( GlobalHandleAdapter::activateGlobalHandle() == false )
		{
			return false;
		}

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

		if( GlobalHandleAdapter::deactivateGlobalHandle() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::_deactivate %s:%s invalid deactivate global handle"
				, this->getType().c_str()
				, this->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_afterDeactivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_update( float _current, float _timing )
	{
        (void)_current;
        (void)_timing;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		bool result = Resource::compile();

		this->updateRendering_();

		if( result == false )
		{
			/*MENGE_LOG_INFO( MENGE_TEXT("Error: compiled Node '%s' is failed\n")
				, getName().c_str() 
				);*/

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
        this->setShallowGrave();

		if( this->isActivate() == true )
		{
			this->deactivate();
		}

		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->release();
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * node = (*it);

				node->release();
			}
		}

		Resource::release();

		this->updateRendering_();

        this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_recompile()
	{
        this->updateRendering_();

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
    void Node::_uncompile()
    {
        this->updateRendering_();

        if( m_enable == false )
        {
            return;
        }

        if( m_active == false )
        {
            return;
        }

        this->deactivate();
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( this->isRenderable() == false )
		{
			return;
		}

		const RenderViewportInterface * renderViewport = _viewport;

		if( m_renderViewport != nullptr )
		{
			renderViewport = m_renderViewport;
		}

		const RenderCameraInterface * renderCamera = _camera;

		if( m_renderCamera != nullptr )
		{
			renderCamera = m_renderCamera;
		}

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
			if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
			{
				this->_render( renderViewport, renderCamera );
			}

			this->renderChild_( renderViewport, renderCamera, _debugMask );
		}
		//}

        if( _debugMask != 0 )
        {
			if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
			{
				this->_debugRender( renderViewport, renderCamera, _debugMask );
			}
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderViewport( const RenderViewportInterface * _viewport )
	{
		m_renderViewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderViewportInterface * Node::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderViewportInterface * Node::getRenderViewportInheritance() const
	{
		const RenderViewportInterface * rv = this->getRenderViewport();

		if( rv != nullptr )
		{
			return rv;
		}

		Node * parent = this->getParent();

		if( parent == nullptr )
		{
			return nullptr;
		}

		const RenderViewportInterface * rv_parent = parent->getRenderViewportInheritance();

		return rv_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderCamera( const RenderCameraInterface * _camera )
	{
		m_renderCamera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderCameraInterface * Node::getRenderCamera() const
	{
		return m_renderCamera;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderCameraInterface * Node::getRenderCameraInheritance() const
	{
		const RenderCameraInterface * rc = this->getRenderCamera();

		if( rc != nullptr )
		{
			return rc;
		}

		Node * parent = this->getParent();

		if( parent == nullptr )
		{
			return nullptr;
		}

		const RenderCameraInterface * rc_parent = parent->getRenderCameraInheritance();

		return rc_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_hide( bool _value )
	{
        (void)_value;

		this->updateRendering_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		for( TListNodeChild::unslug_iterator
			it = m_child.ubegin(),
			it_end = m_child.uend();
		it != it_end;
		++it )
		{
			Node * node = (*it);

			node->render( _viewport, _camera, _debugMask );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
        const ConstString & type = this->getType();

		PyObject * embedding = SCRIPT_SERVICE(m_serviceProvider)
			->wrap( type, this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		this->invalidateBoundingBox();
		//invalidateBoundingBox();add
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::setEventListener( PyObject * _args, PyObject * _kwds )
	{
        (void)_args;

		if( _kwds == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node %s setEventListener wait kwds"
				, m_name.c_str() 
				);

			return pybind::ret_none();
		}
		
		this->_setEventListener( _kwds );

#	ifdef _DEBUG
		size_t pos = 0;

		PyObject * key;
		PyObject * value;
		while( pybind::dict_next( _kwds, pos, &key, &value ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::setEventListener %s:%s invalid event %s:%s"
				, m_name.c_str()
				, m_type.c_str()
				, pybind::object_str( key )
				, pybind::object_str( value )
				);
		}
#	endif
		
        PyObject * py_none = pybind::ret_none();

		return py_none;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeEventListener()
	{
		this->_setEventListener( nullptr );		
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setEventListener( PyObject * _listener )
	{
		GlobalHandleAdapter::setEventListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_embedding( PyObject * _embed )
	{
        (void)_embed;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isEnableGlobalHandle() const
	{
        bool active = this->isActivate();

		return active;
	}
    //////////////////////////////////////////////////////////////////////////
    Eventable * Node::getGlobalHandleEventable()
    {
        Eventable * eventable = static_cast<Eventable *>(this);

        return eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    Scriptable * Node::getGlobalHandleScriptable()
    {
        Scriptable * scriptable = static_cast<Scriptable *>(this);

        return scriptable;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer * _layer )
	{
		m_layer = _layer;

		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->setLayer( _layer );
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * node = (*it);

				node->setLayer( _layer );
			}
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
		if( m_layer == nullptr )
		{
			return nullptr;
		}

		Scene * scene = m_layer->getScene();

		return scene;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::getScreenPosition( const RenderCameraInterface * _camera, mt::vec2f & _position )
	{
        if( m_layer == nullptr )
        {
            const mt::vec3f & pos = this->getWorldPosition();

            _position.x = pos.x;
			_position.y = pos.y;
        }
        else
        {
			m_layer->calcScreenPosition( _position, _camera, this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox ) const
	{
		const mt::vec3f & wp = this->getWorldPosition();

		mt::vec2f v2 = wp.to_vec2f();
		mt::reset( _boundingBox, v2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateColor()
	{
		//ColourValue colour;
		//this->calcTotalColor(colour);

		//bool fullBlend = (colour.getA() < 0.001f);

		//this->setFullBlend( fullBlend );

		Node * single = m_child.single();

		if( single != nullptr )
		{
			single->invalidateColor();
		}
		else
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * node = (*it);

				node->invalidateColor();
			}
		}

		this->updateRendering_();
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Node::getWorldColor() const
	{
		if( m_parent == nullptr )
		{
            const ColourValue & localColor = Colorable::getLocalColor();

			return localColor;
		}

		if( Colorable::isInvalidateColor() == false )
		{
            const ColourValue & relationColor = Colorable::getRelationColor();

			return relationColor;
		}

		const ColourValue & parentColor = m_parent->getWorldColor();

        const ColourValue & relationColor = Colorable::updateRelationColor( parentColor );

		return relationColor;
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
    bool Node::isSolidColor() const
    {
        const ColourValue & worldColour = this->getWorldColor();

        float worldAlpha = worldColour.getA();
        float personalAlpha = this->getPersonalAlpha();

		bool solid = mt::cmp_f_f( worldAlpha * personalAlpha, 1.f );

        return solid;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::visitResource( VisitorResource * _visitor )
	{
        (void)_visitor;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrapInterface * Node::getPickerTrap()
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateRendering_()
	{
		if( this->isCompile() == false )
		{
			m_rendering = false;
		}
		else if( this->isActivate() == false )
		{
			m_rendering = false;
		}
		else if( this->isHide() == true )
		{
			m_rendering = false;
		}
		else if( this->isLocalTransparent() == true )
		{
			m_rendering = false;
		}
		else
		{
			m_rendering = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface* _camera, unsigned int _debugMask )
	{
		if( (_debugMask & MENGE_DEBUG_NODES) == 0 )
		{
			return;
		}

		RenderVertex2D * vertexDebugBox = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( 4 * 2 );

		if( vertexDebugBox == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::_debugRender %s debug vertex overflow"
				, this->getName().c_str()
				);

			return;
		}

		const mt::box2f & bbox = this->getBoundingBox();
			
		vertexDebugBox[0].pos.x = bbox.minimum.x;
		vertexDebugBox[0].pos.y = bbox.minimum.y;

		vertexDebugBox[1].pos.x = bbox.maximum.x;
		vertexDebugBox[1].pos.y = bbox.minimum.y;

		vertexDebugBox[2].pos.x = bbox.maximum.x;
		vertexDebugBox[2].pos.y = bbox.minimum.y;

		vertexDebugBox[3].pos.x = bbox.maximum.x;
		vertexDebugBox[3].pos.y = bbox.maximum.y;

		vertexDebugBox[4].pos.x = bbox.maximum.x;
		vertexDebugBox[4].pos.y = bbox.maximum.y;

		vertexDebugBox[5].pos.x = bbox.minimum.x;
		vertexDebugBox[5].pos.y = bbox.maximum.y;

		vertexDebugBox[6].pos.x = bbox.minimum.x;
		vertexDebugBox[6].pos.y = bbox.maximum.y;

		vertexDebugBox[7].pos.x = bbox.minimum.x;
		vertexDebugBox[7].pos.y = bbox.minimum.y;


		for( uint32_t i = 0; i != 8; ++i )
		{
			vertexDebugBox[i].pos.z = 0.f;

			vertexDebugBox[i].color = 0xFF00FF00;
			vertexDebugBox[i].uv.x = 0.f;
			vertexDebugBox[i].uv.y = 0.f;
			vertexDebugBox[i].uv2.x = 0.f;
			vertexDebugBox[i].uv2.y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();
		
		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
			, vertexDebugBox
			, 8
			, nullptr
			);
	}
}
