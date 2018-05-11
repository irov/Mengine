#include "Kernel/Node.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/NodeInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_active(false)
		, m_deactivating(false)
		, m_afterActive(false)
		, m_enable(true)
		, m_freeze(false)
		, m_speedFactor(1.f)
		, m_rendering(false)
		, m_invalidateRendering(true)
		, m_parent(nullptr)
		, m_renderCamera(nullptr)
		, m_renderViewport(nullptr)
		, m_renderClipplane(nullptr)
		, m_renderTarget(nullptr)
		, m_shallowGrave(0)
		, m_shallowGravePropagate(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node::~Node()
	{		
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

		if( m_shallowGrave == 0 && m_shallowGravePropagate == true )
		{
			this->_unshallowGrave();
		}
    }    
    //////////////////////////////////////////////////////////////////////////
    bool Node::isShallowGrave() const
    {
        return m_shallowGrave > 0;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::setShallowGravePropagate( bool _propagate )
	{
		m_shallowGravePropagate = _propagate;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_unshallowGrave()
	{
		this->setShallowGravePropagate( false );

		Factorable::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::destroy()
	{
		if( this->isShallowGrave() == true )
		{
			NODE_SERVICE()
				->addHomeless( this );

			this->release();

			this->setShallowGravePropagate( true );

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

		m_invalidateRendering = true;

		NodePtr single = m_children.single();

		if( single != nullptr )
		{
			single->activate();
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				NodePtr children = (*it);

				it.next_shuffle();

				children->activate();
			}
		}
	
		m_afterActive = true;

		this->_afterActivate();

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
			LOGGER_ERROR("Node::deactivate %s invalid deactivate in 'activate state'"
				, this->getName().c_str()
				);

			//return;
		}

		m_afterActive = false;

        this->setShallowGrave();

		this->_deactivate();

		NodePtr single = m_children.single();

		if( single != nullptr )
		{
			single->deactivate();
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				NodePtr children = *it;

				it.next_shuffle();

				children->deactivate();
			}
		}

		m_active = false;
		m_deactivating = false;
				
		this->_afterDeactivate();		

		m_invalidateRendering = true;

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
		for( TSlugChild it( m_children ); it.eof() == false; )
		{
			NodePtr node = (*it);

			node->setParent_( nullptr );

			TListNodeChild::iterator it_node( node );

			it.next_shuffle();

			m_children.erase( it_node );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren()
	{
		for( TSlugChild it(m_children); it.eof() == false; )
		{
            NodePtr node = (*it);

			TListNodeChild::iterator it_node( node );

			it.next_shuffle();

            node->deactivate();

            Node * parent = node->getParent();

            if( parent != this )
            {
                continue;
            }

			node->setShallowGrave();

			node->release();

            node->setParent_( nullptr );

			m_children.erase( it_node );

			node->removeShallowGrave();
		}
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
	bool Node::isHomeless() const
	{
		bool result = NODE_SERVICE() 
			->isHomeless( this );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::addChild( const NodePtr & _node )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR("Node::addChildren '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return;
		}

		this->addChild_( m_children.end(), _node );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::addChildFront( const NodePtr & _node )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR("Node::addChildrenFront '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return;
		}

		this->addChild_( m_children.begin(), _node );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildAfter( const NodePtr & _node, const NodePtr & _after )
	{
		if( _node == nullptr )
		{
			LOGGER_ERROR("Node::addChildrenAfter '%s' invalid add NULL node (node)"
				, this->getName().c_str()
				);

			return false;
		}

		if( _after == nullptr )
		{
			LOGGER_ERROR("Node::addChildrenAfter '%s' invalid add NULL node (after)"
				, this->getName().c_str()
				);

			return false;
		}

		TListNodeChild::iterator it_found = 
			stdex::helper::intrusive_find( m_children.begin(), m_children.end(), _after );

		if( it_found == m_children.end() )
		{
			LOGGER_ERROR("Node::addChildrenAfter '%s' after is not child"
				, this->getName().c_str()
				);

			return false;
		}

		this->addChild_( it_found, _node );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void Node::addChild_( TListNodeChild::iterator _insert, const NodePtr & _node )
	{
		Node * parent = _node->getParent();

		this->setShallowGrave();
        _node->setShallowGrave();

		if( parent == this )
		{
			if( *_insert == _node )
			{
				this->removeShallowGrave();
				_node->removeShallowGrave();

				return;
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

			_node->setParent_( this );
		}

		this->_addChild( _node );

		if( this->isFreeze() == false && _node->isFreeze() == true )
		{
			_node->freeze( false );
		}
		else if( this->isFreeze() == true && _node->isFreeze() == false )
		{
			_node->freeze( true );
		}

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

		this->removeShallowGrave();
        _node->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::insertChild_( TListNodeChild::iterator _insert, const NodePtr & _node )
	{
		m_children.insert( _insert, _node );
	}
	//////////////////////////////////////////////////////////////////////////
    void Node::eraseChild_( const NodePtr & _node )
	{
        TListNodeChild::iterator it_node(_node, true);
		m_children.erase( it_node );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		this->visit( _visitor );

		this->setShallowGrave();

		NodePtr single_child = m_children.single();

		if( single_child != nullptr )
		{
			single_child->visitChildren( _visitor );
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
                NodePtr children = (*it);

				it.next_shuffle();

				children->visitChildren( _visitor );
			}
		}

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::removeChild( const NodePtr & _node )
	{
#ifndef NDEBUG
		if( stdex::helper::intrusive_has( m_children.begin(), m_children.end(), _node ) == false )
        {
            LOGGER_ERROR("Node::removeChild %s not found children %s"
                , this->getName().c_str()
                , _node->getName().c_str()
                );

            return false;
        }
#endif

        _node->deactivate();

		//check if deactivate remove from parent!
		Node * parent = _node->getParent();

		if( parent != this )
		{
			return false;
		}

        this->removeChild_( _node );
		
		//NODE_SERVICE()
		//	->addHomeless( _node );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::removeChild_( const NodePtr & _node )
    {
        this->_removeChild( _node );
		
		_node->setParent_( nullptr );

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
            bool operator () ( const NodePtr & _node ) const
			{
                Node * node_ptr = _node.get();

                return (node_ptr->*m_method)() == m_name;
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
				stdex::helper::intrusive_find_if( _child.begin(), _child.end(), FFindChild<M>( _method, _name) );

			return it_found;
		}
	}
	//////////////////////////////////////////////////////////////////////////
    NodePtr Node::findChild( const ConstString & _name, bool _recursion ) const
	{
		TListNodeChild::const_iterator it_found =
			s_node_find_child( m_children, &Identity::getName, _name );

		if( it_found != m_children.end() )
		{
            NodePtr children = *it_found;

			return children;
		}

		if( _recursion == true )
		{
			for( TListNodeChild::const_iterator 
				it = m_children.begin(), 
				it_end = m_children.end();
			it != it_end;
			++it )
			{
                const NodePtr & children = (*it);

                NodePtr node = children->findChild( _name, true );

				if( node == nullptr )
				{
					continue;
				}

				return node;
			}
		}
		else
		{
            NodePtr node = this->_findChild( _name, _recursion );

			if( node != nullptr )
			{
				return node;
			}
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
    NodePtr Node::getSiblingPrev()
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }
        
        TListNodeChild & parent_children = parent->getChildren();

        TListNodeChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), this );

        if( it_found == parent_children.begin() )
        {
            return nullptr;
        }

        it_found--;

        NodePtr prev_node = *it_found;

        return prev_node;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::getSiblingNext()
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }

        TListNodeChild & parent_children = parent->getChildren();

        TListNodeChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), this );

        it_found++;

        if( it_found == parent_children.end() )
        {
            return nullptr;
        }

        NodePtr next_node = *it_found;

        return next_node;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Node::hasChild( const ConstString & _name, bool _recursive ) const
	{
		TListNodeChild::const_iterator it_found =
			s_node_find_child( m_children, &Identity::getName, _name );

		if( it_found != m_children.end() )
		{
			return true;
		}

		if( _recursive == true )
		{		
			for( TListNodeChild::const_iterator 
				it = m_children.begin(), 
				it_end = m_children.end();
			it != it_end;
			++it )
			{
                NodePtr children = (*it);
				
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
		return m_children.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_changeParent( Node * _oldParent, Node * _newParent )
	{
        (void)_oldParent;
        (void)_newParent;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_addChild( const NodePtr & _node )
	{
        (void)_node;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_removeChild( const NodePtr & _node )
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
		this->setShallowGrave();

		for( TSlugChild it(m_children); it.eof() == false; )
		{
			NodePtr node = *it;

			it.next_shuffle();

			node->freeze( _value );
		}

		this->removeShallowGrave();
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
		
		this->updateChildren_( _current, total_timing );

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateChildren_( float _current, float _timing )
	{
		NodePtr single = m_children.single();

		if( single != nullptr )
		{
			single->update( _current, _timing );
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				NodePtr children = *it;

				it.next_shuffle();
			
				children->update( _current, _timing );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
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

		if( m_invalidateWorldMatrix == true )
		{
			this->updateWorldMatrix();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		bool result = Resource::compile();

		m_invalidateRendering = true;

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
        this->setShallowGrave();

		if( this->isActivate() == true )
		{
			this->deactivate();
		}

		NodePtr single = m_children.single();

		if( single != nullptr )
		{
			single->release();
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				NodePtr node = (*it);

				it.next_shuffle();

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
		m_invalidateRendering = true;

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
		m_invalidateRendering = true;

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
	void Node::render( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask )
	{
		if( this->isRenderable() == false )
		{
			return;
		}

		RenderViewportInterfacePtr renderViewport = _state->viewport;

		if( m_renderViewport != nullptr )
		{
			renderViewport = m_renderViewport;
		}

		RenderCameraInterfacePtr renderCamera = _state->camera;

		if( m_renderCamera != nullptr )
		{
			renderCamera = m_renderCamera;
		}

		RenderClipplaneInterfacePtr renderClipplane = _state->clipplane;

		if( m_renderClipplane != nullptr )
		{
			renderClipplane = m_renderClipplane;
		}

		RenderTargetInterfacePtr renderTarget = _state->target;

		if( m_renderTarget != nullptr )
		{
			renderTarget = m_renderTarget;
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

		RenderState state;
		state.viewport = renderViewport;
		state.camera = renderCamera;
		state.clipplane = renderClipplane;
		state.target = renderTarget;

		//if( this->checkVisibility( viewPort ) == true )
		{
			if( this->getLocalHide() == false && this->isPersonalTransparent() == false )
			{
				this->_render( _renderService, &state );
			}

			this->renderChild_( _renderService, &state, _debugMask );
		}
		//}

        if( m_renderTarget != nullptr )
        {
            this->_renderTarget( _renderService, _state, _debugMask );
        }

        if( _debugMask != 0 )
        {
			if( this->getLocalHide() == false && this->isPersonalTransparent() == false )
			{
				this->_debugRender( _renderService, &state, _debugMask );
			}
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::_renderTarget( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask )
    {
        (void)_renderService;
        (void)_state;
        (void)_debugMask;

        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
	{
		m_renderViewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderViewportInterfacePtr & Node::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderViewportInterfacePtr & Node::getRenderViewportInheritance() const
	{
		const RenderViewportInterfacePtr & rv = this->getRenderViewport();

		if( rv != nullptr )
		{
			return rv;
		}

		Node * parent = this->getParent();

		if( parent == nullptr )
		{
			return RenderViewportInterfacePtr::none();
		}

		const RenderViewportInterfacePtr & rv_parent = parent->getRenderViewportInheritance();

		return rv_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderCamera( const RenderCameraInterfacePtr & _camera )
	{
		m_renderCamera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderCameraInterfacePtr & Node::getRenderCamera() const
	{
		return m_renderCamera;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderCameraInterfacePtr & Node::getRenderCameraInheritance() const
	{
		const RenderCameraInterfacePtr & rc = this->getRenderCamera();

		if( rc != nullptr )
		{
			return rc;
		}

		Node * parent = this->getParent();

		if( parent == nullptr )
		{
			return RenderCameraInterfacePtr::none();
		}

		const RenderCameraInterfacePtr & rc_parent = parent->getRenderCameraInheritance();

		return rc_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderClipplane( const RenderClipplaneInterfacePtr & _clipplane )
	{
		m_renderClipplane = _clipplane;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderClipplaneInterfacePtr & Node::getRenderClipplane() const
	{
		return m_renderClipplane;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderClipplaneInterfacePtr & Node::getRenderClipplaneInheritance() const
	{
		const RenderClipplaneInterfacePtr & rc = this->getRenderClipplane();

		if( rc != nullptr )
		{
			return rc;
		}

		Node * parent = this->getParent();

		if( parent == nullptr )
		{
            return RenderClipplaneInterfacePtr::none();
		}

		const RenderClipplaneInterfacePtr & rc_parent = parent->getRenderClipplaneInheritance();

		return rc_parent;
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::setRenderTarget( const RenderTargetInterfacePtr & _target )
    {
        m_renderTarget = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & Node::getRenderTarget() const
    {
        return m_renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & Node::getRenderTargetInheritance() const
    {
        const RenderTargetInterfacePtr & rt = this->getRenderTarget();

        if( rt != nullptr )
        {
            return rt;
        }

        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return RenderTargetInterfacePtr::none();
        }

        const RenderTargetInterfacePtr & rt_parent = parent->getRenderTargetInheritance();

        return rt_parent;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::_setHide( bool _value )
	{
        (void)_value;

		m_invalidateRendering = true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::_setExternalRender( bool _value )
    {
        (void)_value;

        m_invalidateRendering = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild_( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask )
	{
		for( TListNodeChild::unslug_iterator
			it = m_children.ubegin(),
			it_end = m_children.uend();
		it != it_end;
		++it )
		{
            NodePtr node = (*it);

            if( node->getExternalRender() == true )
            {
                continue;
            }

			node->render( _renderService, _state, _debugMask );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		this->invalidateBoundingBox();
		//invalidateBoundingBox();add
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::calcScreenPosition( const RenderCameraInterfacePtr & _camera, mt::vec2f & _screen )
	{
        const mt::vec3f & wp = this->getWorldPosition();

        const mt::mat4f & vm = _camera->getCameraViewMatrix();

        mt::vec3f sc;
        mt::mul_m4_v3( sc, vm, wp );

        _screen.x = sc.x;
        _screen.y = sc.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox ) const
	{
		_boundingBox.minimum.x = -(std::numeric_limits<float>::max)();
		_boundingBox.minimum.y = -(std::numeric_limits<float>::max)();
		_boundingBox.maximum.x = (std::numeric_limits<float>::max)();
		_boundingBox.maximum.y = (std::numeric_limits<float>::max)();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateColor()
	{
		NodePtr single = m_children.single();

		if( single != nullptr )
		{
			single->invalidateColor();
		}
		else
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
                NodePtr node = (*it);

				it.next_shuffle();

				node->invalidateColor();
			}
		}

		m_invalidateRendering = true;
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

		bool solid = mt::equal_f_f( worldAlpha * personalAlpha, 1.f );

        return solid;
    }
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrapInterface * Node::getPickerTrap()
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateRendering_() const
	{
		m_invalidateRendering = false;

		if( this->isCompile() == false )
		{
			m_rendering = false;
		}
		else if( this->isActivate() == false )
		{
			m_rendering = false;
		}
		else if( this->getHide() == true )
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
	void Node::_debugRender( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask )
	{
        if( (_debugMask & MENGINE_DEBUG_NODES) == 0 )
		{
			return;
		}

		RenderVertex2D * vertices = RENDER_SERVICE()
			->getDebugRenderVertex2D( 4 * 2 );

		if( vertices == nullptr )
		{
			LOGGER_ERROR("Node::_debugRender %s debug vertex overflow"
				, this->getName().c_str()
				);

			return;
		}

		const mt::box2f & bbox = this->getBoundingBox();
			
		vertices[0].position.x = bbox.minimum.x;
		vertices[0].position.y = bbox.minimum.y;

		vertices[1].position.x = bbox.maximum.x;
		vertices[1].position.y = bbox.minimum.y;

		vertices[2].position.x = bbox.maximum.x;
		vertices[2].position.y = bbox.minimum.y;

		vertices[3].position.x = bbox.maximum.x;
		vertices[3].position.y = bbox.maximum.y;

		vertices[4].position.x = bbox.maximum.x;
		vertices[4].position.y = bbox.maximum.y;

		vertices[5].position.x = bbox.minimum.x;
		vertices[5].position.y = bbox.maximum.y;

		vertices[6].position.x = bbox.minimum.x;
		vertices[6].position.y = bbox.maximum.y;

		vertices[7].position.x = bbox.minimum.x;
		vertices[7].position.y = bbox.minimum.y;


		for( uint32_t i = 0; i != 8; ++i )
		{
			vertices[i].position.z = 0.f;

			vertices[i].color = 0xFF00FF00;
			vertices[i].uv[0].x = 0.f;
			vertices[i].uv[0].y = 0.f;
			vertices[i].uv[1].x = 0.f;
			vertices[i].uv[1].y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
			->getDebugMaterial();
		
		_renderService
			->addRenderLine( _state, debugMaterial
			, vertices
			, 8
			, nullptr
			, true
			);
	}
}
