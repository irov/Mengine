#	include "Kernel/Node.h"

#   include "Kernel/Layer.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/NodeInterface.h"

#	include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_serviceProvider(NULL)
        , m_active(false)
		, m_enable(true)
		, m_freeze(false)
		, m_rendering(false)
		, m_parent(0)
		, m_layer(0)
		, m_cameraRevision(0)
		, m_renderCamera(0)
		, m_shallowGrave(0)
#	ifndef MENGE_MASTER_RELEASE
		, m_debugMaterial(NULL)
#	endif
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
	bool Node::_destroy()
	{
		if( this->isShallowGrave() == true )
		{
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

			return false;
		}

        this->release();

        this->destroyAllChild();

        this->removeFromParent();

        this->unwrap();
        
		return true;
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

        this->setShallowGrave();

		m_active = true;

		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
			Node * children = (*it);

			children->activate();
		}
	
		this->_afterActivate();

		//if( m_active == false )
		//{
		//	MENGE_LOG_INFO( "Error: activation of Node '%s' is failed\n"
		//		, m_name.c_str() 
		//		);
		//}

		this->updateRendering_();

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

        this->setShallowGrave();

		m_cameraRevision = 0;

		this->_deactivate();

		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
			Node * children = *it;

			children->deactivate();
		}

		m_active = false;

		//if( this->isCompile() == true )
		//{
		//	this->release();
		//}

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
        this->setShallowGrave();

		if( this->isActivate() == true )
		{
			this->deactivate();
		}

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

            //NODE_SERVICE(m_serviceProvider)
            //    ->addHomeless( node );            

			node->setParent_( NULL );
            //node->setLayer( NULL );

			TListNodeChild::iterator it_node(node);

			it.next_shuffle();

			m_child.erase( it_node );

            node->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeAllChild()
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

            node->setParent_( NULL );
            node->setLayer( NULL );
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

		//this->disable();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren( Node * _node )
	{
		if( _node == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Node::addChildren '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return false;
		}

		bool result = this->addChildren_( m_child.end(), _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenFront( Node * _node )
	{
		if( _node == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Node::addChildrenFront '%s' invalid add NULL node"
				, this->getName().c_str()
				);

			return false;
		}

		bool result = this->addChildren_( m_child.begin(), _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenAfter( Node* _node, Node * _after )
	{
		if( _node == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Node::addChildrenAfter '%s' invalid add NULL node (node)"
				, this->getName().c_str()
				);

			return false;
		}

		if( _after == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Node::addChildrenAfter '%s' invalid add NULL node (after)"
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

		bool result = this->addChildren_( it_found, _node );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren_( TListNodeChild::iterator _insert, Node * _node )
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

        _node->setShallowGrave();

		if( parent == this )
		{
			this->eraseChildren_( _node );
			this->insertChildren_( _insert, _node );			
		}
		else
		{
			if( parent )
			{
				parent->removeChildren_( _node );
			}

			this->insertChildren_( _insert, _node );

            _node->setLayer( m_layer );
			_node->setParent_( this );
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

		this->_addChildren( _node );

        _node->removeShallowGrave();

		//this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::insertChildren_( TListNodeChild::iterator _insert, Node * _node )
	{
		m_child.insert( _insert, _node );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::eraseChildren_( Node * _node )
	{
        TListNodeChild::iterator it_node(_node, true);
		m_child.erase( it_node );
	}
	//////////////////////////////////////////////////////////////////////////
	const TListNodeChild & Node::getChild() const
	{
		return m_child;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
        if( intrusive_has( m_child.begin(), m_child.end(), _node ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Node::removeChildren %s not found children %s"
                , this->getName().c_str()
                , _node->getName().c_str()
                );

            return;
        }

        _node->deactivate();

        Node * parent = _node->getParent();

        if( parent != this )
        {
            return;
        }    

        this->removeChildren_( _node );
	}
    //////////////////////////////////////////////////////////////////////////
    void Node::removeChildren_( Node * _node )
    {
        this->_removeChildren( _node );

        _node->setParent_( NULL );
        _node->setLayer( NULL );

        this->eraseChildren_( _node );
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

        private:
            FFindChild & operator = ( const FFindChild & _child )
            {
                (void)_child;

                return *this;
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
		TListNodeChild::const_iterator it_found =
			Impl::find_child( m_child, &Identity::getName, _name );

		if( it_found != m_child.end() )
		{
			return *it_found;
		}

		if( _recursion == true )
		{
			for( TListNodeChild::const_iterator 
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
		TListNodeChild::const_iterator it_found =
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
        (void)_name;
        (void)_recursion;

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::hasChildren( const ConstString & _name, bool _recursive ) const
	{
		TListNodeChild::const_iterator it_found =
			Impl::find_child( m_child, &Identity::getName, _name );

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
	bool Node::_hasChildren( const ConstString & _name, bool _recursive ) const
	{
        (void)_name;
        (void)_recursive;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::emptyChild() const
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
	void Node::_addChildren( Node * _node )
	{
        (void)_node;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_removeChildren( Node * _node )
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

			if( node->isFreeze() != _value )
			{
				(*it)->_freeze( _value );
			}
		}
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

		this->setShallowGrave();

		this->_update( _current, _timing );

		Affectorable::update( _current, _timing );
		
		this->updateChild_( _current, _timing );

		this->removeShallowGrave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::updateChild_( float _current, float _timing )
	{
		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
			Node * children = *it;
			
			children->update( _current, _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
		GlobalHandleAdapter::activateGlobalHandle();

//#ifndef MENGE_MASTER_RELEASE
		const RenderMaterialGroup * mg_debug = RENDER_SERVICE(m_serviceProvider)
            ->getMaterialGroup( Helper::stringizeString(m_serviceProvider, "Debug") );

		m_debugMaterial = mg_debug->getMaterial( TAM_CLAMP, TAM_CLAMP );

		if( m_debugMaterial == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("Node::_activate %s m_debugMaterial Debug not found"
				, this->getName().c_str()
				);

			return false;
		}

		ApplyColor2D applyColor( 0xFF00FF00 );

		std::for_each( m_debugBox, m_debugBox + 4, applyColor );
//#endif // MENGE_MASTER_RELEASE

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

//#ifndef MENGE_MASTER_RELEASE
		m_debugMaterial = 0;
//#endif // MENGE_MASTER_RELEASE
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

		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
            Node * node = (*it);

			node->release();
		}

		Resource::release();

		this->updateRendering_();

        this->removeShallowGrave();
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
	void Node::render( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( this->isRenderable() == false )
		{
			return;
		}

		RenderCameraInterface * renderCamera = _camera;

		if( m_renderCamera != NULL )
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
				this->_render( renderCamera );
			}

			this->renderChild( renderCamera, _debugMask );
		}
		//}

        if( _debugMask != 0 )
        {
            this->_debugRender( _camera, _debugMask );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setRenderCamera( RenderCameraInterface * _camera )
	{
		m_renderCamera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderCameraInterface * Node::getRenderCamera() const
	{
		return m_renderCamera;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_hide( bool _value )
	{
        (void)_value;

		this->updateRendering_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
            Node * node = (*it);

			node->render( _camera, _debugMask );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
        (void)_viewport;
		//const mt::box2f & bbox = this->getBoundingBox();

		//bool result = _viewport.testBBox( bbox );

		//return result;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = SCRIPT_SERVICE(m_serviceProvider)
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		//invalidateBoundingBox();add
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::setEventListener( PyObject * _args, PyObject * _kwds )
	{
        (void)_args;

		if( _kwds == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Node %s setEventListener wait kwds"
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
        (void)_embed;
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

		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
            Node * node = (*it);

			node->setLayer( _layer );
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
	void Node::getCameraPosition( RenderCameraInterface * _camera, mt::vec2f & _position )
	{
        if( m_layer == NULL )
        {
            const mt::vec3f & pos = this->getWorldPosition();

            _position = pos.to_vec2f();
        }
        else
        {
			const Viewport & viewport = _camera->getViewport();

			m_layer->calcScreenPosition( _position, viewport, this );
		}
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
			
		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
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
		//ColourValue colour;
		//this->calcTotalColor(colour);

		//bool fullBlend = (colour.getA() < 0.001f);

		//this->setFullBlend( fullBlend );

		for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
		{
            Node * node = (*it);

			node->invalidateColor();
		}

		this->updateRendering_();
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
        (void)_visitor;
		//Empty
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
//#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Node::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_NODES )
		{
			mt::box2f bbox;
			this->getBoundingBox( bbox );
			
			m_debugBox[0].pos[0] = bbox.minimum.x;
			m_debugBox[0].pos[1] = bbox.minimum.y;

			m_debugBox[1].pos[0] = bbox.maximum.x;
			m_debugBox[1].pos[1] = bbox.minimum.y;
	

			m_debugBox[2].pos[0] = bbox.maximum.x;
			m_debugBox[2].pos[1] = bbox.maximum.y;

			m_debugBox[3].pos[0] = bbox.minimum.x;
			m_debugBox[3].pos[1] = bbox.maximum.y;


			m_debugBox[4].pos[0] = bbox.minimum.x;
			m_debugBox[4].pos[1] = bbox.minimum.y;

            for( size_t i = 0; i != 5; ++i )
            {
			    m_debugBox[i].pos[2] = 0.f;
                m_debugBox[i].color = 0xFFFFFFFF;
                m_debugBox[i].uv[0] = 0.f;
                m_debugBox[i].uv[1] = 0.f;
                m_debugBox[i].uv2[0] = 0.f;
                m_debugBox[i].uv2[1] = 0.f;
            }

			RENDER_SERVICE(m_serviceProvider)
				->addRenderObject2D( _camera, m_debugMaterial, NULL, 0, m_debugBox, 5, LPT_LINE );
		}
	}
//#	endif
}
