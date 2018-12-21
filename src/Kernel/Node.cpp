#include "Kernel/Node.h"

#include "Interface/TimelineServiceInterface.h"
#include "Interface/PickerServiceInterface.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/NodeRenderHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Node::Node()
        : m_active( false )
        , m_deactivating( false )
        , m_afterActive( false )
        , m_enable( true )
        , m_freeze( false )
        //, m_rendering( false )
        //, m_invalidateRendering( true )
        , m_uniqueIdentity( 0 )
        , m_parent( nullptr )
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

        this->destroyChildren( []( const NodePtr & ) {} );

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

        stdex::intrusive_this_acquire( this );

        m_active = true;

        //m_invalidateRendering = true;

        if( m_children.empty() == false )
        {
            NodePtr single = m_children.single();

            if( single != nullptr )
            {
                single->activate();
            }
            else
            {
                for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
                {
                    const NodePtr & children = (*it);

                    it.next_shuffle();

                    children->activate();
                }
            }
        }

        if( m_active == true )
        {
            m_afterActive = true;

            this->_afterActivate();
        }
        else
        {
            m_active = false;
        }

        stdex::intrusive_this_release( this );

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
            LOGGER_ERROR( "Node::deactivate %s invalid deactivate in 'activate state'"
                , this->getName().c_str()
            );

            //return;
        }

        m_afterActive = false;

        this->_deactivate();

        if( m_children.empty() == false )
        {
            NodePtr single = m_children.single();

            if( single != nullptr )
            {
                single->deactivate();
            }
            else
            {
                for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
                {
                    const NodePtr & children = *it;

                    it.next_shuffle();

                    children->deactivate();
                }
            }
        }

        m_active = false;
        m_deactivating = false;

        this->_afterDeactivate();

        //m_invalidateRendering = true;
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
        else
        {
            return true;
        }

        if( this->activate() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::enableForce()
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
        stdex::intrusive_this_acquire( this );

        this->deactivate();

        m_enable = false;

        stdex::intrusive_this_release( this );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getLeafDeep() const
    {
        uint32_t deep = 0;

        Node * parent = m_parent;

        while( parent != nullptr )
        {
            ++deep;

            parent = parent->getParent();
        }

        return deep;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeParentRender_()
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * oldRenderParent = render->getRelationRender();

            if( oldRenderParent != nullptr )
            {
                render->removeRelationRender();
                render->invalidateColor();
            }
        }
        else
        {
            RenderInterface * oldRenderParent = Helper::getNodeRenderInheritance( m_parent );

            if( oldRenderParent != nullptr )
            {
                this->foreachRenderCloseChildren( []( RenderInterface * _render )
                {
                    _render->removeRelationRender();
                    _render->invalidateColor();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setParentRender_( Node * _parent )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * oldRenderParent = render->getRelationRender();
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                render->setRelationRender( newRenderParent );
                render->invalidateColor();
            }
        }
        else
        {
            RenderInterface * oldRenderParent = Helper::getNodeRenderInheritance( m_parent );
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                this->foreachRenderCloseChildren( [newRenderParent]( RenderInterface * _render )
                {
                    _render->setRelationRender( newRenderParent );
                    _render->invalidateColor();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeParent_()
    {
        Node * oldparent = m_parent;
        m_parent = nullptr;

        this->removeRelationTransformation();
        this->setParentRender_( nullptr );

        this->_changeParent( oldparent, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setParent_( Node * _parent )
    {
        Node * oldparent = m_parent;
        m_parent = _parent;

        this->setRelationTransformation( _parent );
        this->setParentRender_( _parent );

        UpdationInterface * updation = this->getUpdation();

        if( _parent != nullptr && updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->replace( deep );
        }

        this->_changeParent( oldparent, _parent );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::destroyChildren( const LambdaDestroyChildren & _lambda )
    {
        if( m_children.empty() == true )
        {
            return;
        }

        for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
        {
            const NodePtr & node = (*it);

            it.next_shuffle();

            node->deactivate();

            Node * parent = node->getParent();

            if( parent != this )
            {
                continue;
            }

            node->release();

            node->removeParent_();

            IntrusiveSlugListNodeChild::iterator it_node( node );

            m_children.erase( it_node );

            _lambda( node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeChildren( const LambdaRemoveChildren & _lambda )
    {
        if( m_children.empty() == true )
        {
            return;
        }

        for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
        {
            const NodePtr & node = (*it);

            IntrusiveSlugListNodeChild::iterator it_node( node );

            it.next_shuffle();

            node->deactivate();

            Node * parent = node->getParent();

            if( parent != this )
            {
                continue;
            }

            node->release();

            node->removeParent_();

            m_children.erase( it_node );

            _lambda( node );
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
    void Node::setUniqueIdentity( uint32_t _uniqueIdentity )
    {
        m_uniqueIdentity = _uniqueIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getUniqueIdentity() const
    {
        return m_uniqueIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addChild( const NodePtr & _node )
    {
        MENGINE_ASSERTION( _node != nullptr, ("node '%s' invalid add child NULL node"
            , this->getName().c_str()
            ) );

        this->addChild_( m_children.end(), _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addChildFront( const NodePtr & _node )
    {
        MENGINE_ASSERTION( _node != nullptr, ("node '%s' invalid add front child NULL node"
            , this->getName().c_str()
            ) );

        this->addChild_( m_children.begin(), _node );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::addChildAfter( const NodePtr & _node, const NodePtr & _after )
    {
        MENGINE_ASSERTION( _node != nullptr, ("node '%s' invalid add child NULL node (node)"
            , this->getName().c_str()
            ) );

        MENGINE_ASSERTION( _after != nullptr, ("node '%s' invalid add after NULL node (node)"
            , this->getName().c_str()
            ) );

        MENGINE_ASSERTION( _node != _after, ("node '%s' invalid add child '%s' is equal after '%s' (node)"
            , this->getName().c_str()
            , _node->getName().c_str()
            , _after->getName().c_str()
            ) );

        IntrusiveSlugListNodeChild::iterator it_found =
            stdex::helper::intrusive_find( m_children.begin(), m_children.end(), _after );

        MENGINE_ASSERTION( it_found != m_children.end(), ("node '%s' after is not child"
            , this->getName().c_str()
            ) );

        this->addChild_( it_found, _node );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _child )
    {
        stdex::intrusive_this_acquire( this );

        Node * child_parent = _child->getParent();

        if( child_parent == this )
        {
            if( *_insert == _child )
            {
                stdex::intrusive_this_release( this );

                return;
            }

            this->eraseChild_( _child );

            this->insertChild_( _insert, _child );
        }
        else
        {
            if( child_parent != nullptr )
            {
                child_parent->removeChild_( _child );
            }

            this->insertChild_( _insert, _child );

            _child->setParent_( this );
        }

        this->_addChild( _child );

        if( this->isFreeze() == false && _child->isFreeze() == true )
        {
            _child->freeze( false );
        }
        else if( this->isFreeze() == true && _child->isFreeze() == false )
        {
            _child->freeze( true );
        }

        if( this->isActivate() == false && _child->isActivate() == true )
        {
            _child->deactivate();
        }
        else if( this->isActivate() == true && _child->isActivate() == false )
        {
            _child->activate();
        }

        _child->invalidateWorldMatrix();

        stdex::intrusive_this_release( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::insertChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _node )
    {
        m_children.insert( _insert, _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::eraseChild_( const NodePtr & _node )
    {
        IntrusiveSlugListNodeChild::iterator it_node( _node, true );
        m_children.erase( it_node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachChildren( const LambdaNode & _lambda ) const
    {
        for( IntrusiveSlugListNodeChild::const_iterator
            it = m_children.begin(),
            it_end = m_children.end();
            it != it_end;
            ++it )
        {
            const NodePtr & child = (*it);

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachChildrenUnslug( const LambdaNode & _lambda ) const
    {
        for( IntrusiveSlugListNodeChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            NodePtr child = (*it);

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachChildrenUnslugBreak( const LambdaNodeBreak & _lambda ) const
    {
        for( IntrusiveSlugListNodeChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            NodePtr child = (*it);

            if( _lambda( child ) == false )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::findUniqueChild( uint32_t _uniqueIdentity ) const
    {
        for( IntrusiveSlugListNodeChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            NodePtr child = (*it);

            uint32_t childUID = child->getUniqueIdentity();

            if( childUID != _uniqueIdentity )
            {
                continue;
            }

            return child;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachRenderCloseChildren( const LambdaNodeRenderCloseChildren & _lambda )
    {
        this->foreachChildren( [&_lambda]( const NodePtr & _child )
        {
            RenderInterface * render = _child->getRender();

            if( render != nullptr )
            {
                _lambda( render );
            }
            else
            {
                _child->foreachRenderCloseChildren( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::visitChildren( const VisitorPtr & _visitor )
    {
        this->visit( _visitor );

        if( m_children.empty() == true )
        {
            return;
        }

        stdex::intrusive_this_acquire( this );

        NodePtr single_child = m_children.single();

        if( single_child != nullptr )
        {
            single_child->visitChildren( _visitor );
        }
        else
        {
            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                const NodePtr & children = (*it);

                it.next_shuffle();

                children->visitChildren( _visitor );
            }
        }

        stdex::intrusive_this_release( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::absorbBoundingBox( mt::box2f & _bb )
    {
        bool successul = false;

        mt::box2f absorb_bb;
        mt::insideout_box( absorb_bb );

        const mt::box2f * bb = this->getBoundingBox();

        if( bb != nullptr )
        {
            mt::merge_box( absorb_bb, *bb );

            successul = true;
        }

        this->foreachChildren( [&absorb_bb, &successul]( const NodePtr & _child )
        {
            mt::box2f child_bb;
            if( _child->absorbBoundingBox( child_bb ) == true )
            {
                successul = true;
            }

            mt::merge_box( absorb_bb, child_bb );
        } );

        _bb = absorb_bb;

        return successul;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::removeChild( const NodePtr & _node )
    {
#ifndef NDEBUG
        if( stdex::helper::intrusive_has( m_children.begin(), m_children.end(), _node ) == false )
        {
            LOGGER_ERROR( "Node::removeChild %s not found children %s"
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeChild_( const NodePtr & _node )
    {
        this->_removeChild( _node );

        _node->removeParent_();

        this->eraseChild_( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getChildrenRecursiveCount() const
    {
        uint32_t size = m_children.size();

        this->foreachChildren( [&size]( const NodePtr & _child )
        {
            size += _child->getChildrenRecursiveCount();
        } );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        template<class M>
        class FFindChild
        {
        public:
            FFindChild( M _method, const ConstString & _name )
                : m_method( _method )
                , m_name( _name )
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
                stdex::helper::intrusive_find_if( _child.begin(), _child.end(), FFindChild<M>( _method, _name ) );

            return it_found;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::findChild( const ConstString & _name, bool _recursion ) const
    {
        IntrusiveSlugListNodeChild::const_iterator it_found =
            s_node_find_child( m_children, &Identity::getName, _name );

        if( it_found != m_children.end() )
        {
            NodePtr children = *it_found;

            return children;
        }

        if( _recursion == true )
        {
            for( IntrusiveSlugListNodeChild::const_iterator
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
    NodePtr Node::_findChild( const ConstString & _name, bool _recursion ) const
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

        IntrusiveSlugListNodeChild & parent_children = parent->getChildren();

        IntrusiveSlugListNodeChild::iterator it_found =
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

        IntrusiveSlugListNodeChild & parent_children = parent->getChildren();

        IntrusiveSlugListNodeChild::iterator it_found =
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
        IntrusiveSlugListNodeChild::const_iterator it_found =
            s_node_find_child( m_children, &Identity::getName, _name );

        if( it_found != m_children.end() )
        {
            return true;
        }

        if( _recursive == true )
        {
            for( IntrusiveSlugListNodeChild::const_iterator
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
        if( m_children.empty() != true )
        {
            stdex::intrusive_this_acquire( this );

            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                const NodePtr & node = *it;

                it.next_shuffle();

                node->freeze( _value );
            }

            stdex::intrusive_this_release( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setSpeedFactor( float _speedFactor )
    {
        (void)_speedFactor;
        //TODO: REMOVE
    }
    //////////////////////////////////////////////////////////////////////////
    float Node::getSpeedFactor() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    //void Node::update( const UpdateContext * _context )
    //{
    //    if( this->isActivate() == false )
    //    {
    //        return;
    //    }

    //    if( this->isFreeze() == true )
    //    {
    //        return;
    //    }

    //    this->setUpdateRevision( _context->revision );

    //    stdex::intrusive_this_acquire( this );

    //    this->_update( _context );

        //Affectorable::updateAffectors( _context );

    //    this->updateChildren_( _context );

    //    stdex::intrusive_this_release( this );
    //}
    ////////////////////////////////////////////////////////////////////////////
    //void Node::updateChildren_( const UpdateContext * _context )
    //{
    //    if( m_children.empty() == true )
    //    {
    //        return;
    //    }

    //    NodePtr single = m_children.single();

    //    if( single != nullptr )
    //    {
    //        single->update( _context );
    //    }
    //    else
    //    {
    //        for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
    //        {
    //            NodePtr children = *it;

    //            it.next_shuffle();

    //            children->update( _context );
    //        }
    //    }
    //}
    //////////////////////////////////////////////////////////////////////////
    bool Node::_activate()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_afterActivate()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->activate( 0U, deep );
        }

        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->setRenderEnable( true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_deactivate()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation != nullptr )
        {
            updation->deactivate();
        }

        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->setRenderEnable( false );
        }

        Affectorable::stopAllAffectors();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_afterDeactivate()
    {
        //Empty
    }
    ////////////////////////////////////////////////////////////////////////////
    //void Node::_update( const UpdateContext * _context )
    //{
    //    (void)_context;

    //    if( m_invalidateWorldMatrix == true )
    //    {
    //        this->updateWorldMatrix();
    //    }
    //}
    //////////////////////////////////////////////////////////////////////////
    bool Node::compile()
    {
        bool result = Compilable::compile();

        //m_invalidateRendering = true;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::release()
    {
        if( this->isActivate() == true )
        {
            this->deactivate();
        }

        if( m_children.empty() == false )
        {
            NodePtr single = m_children.single();

            if( single != nullptr )
            {
                single->release();
            }
            else
            {
                for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
                {
                    const NodePtr & node = (*it);

                    it.next_shuffle();

                    node->release();
                }
            }
        }

        Compilable::release();

        //this->updateRendering_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_recompile()
    {
        //m_invalidateRendering = true;

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
        //m_invalidateRendering = true;

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
    ////////////////////////////////////////////////////////////////////////////
    //void Node::render( const RenderContext * _state )
    //{
    //    if( this->isRenderable() == false )
    //    {
    //        return;
    //    }

    //    RenderViewportInterfacePtr renderViewport = _state->viewport;

    //    if( m_renderViewport != nullptr )
    //    {
    //        renderViewport = m_renderViewport;
    //    }

    //    RenderCameraInterfacePtr renderCamera = _state->camera;

    //    if( m_renderCamera != nullptr )
    //    {
    //        renderCamera = m_renderCamera;
    //    }

    //    RenderScissorInterfacePtr renderScissor = _state->scissor;

    //    if( m_renderScissor != nullptr )
    //    {
    //        renderScissor = m_renderScissor;
    //    }

    //    RenderTargetInterfacePtr renderTarget = _state->target;

    //    if( m_renderTarget != nullptr )
    //    {
    //        renderTarget = m_renderTarget;
    //    }

    //    //const Viewport& viewPort = _camera->getViewport();

    //    //size_t cameraRevision = _camera->getCameraRevision();

    //    //if( m_cameraRevision == cameraRevision && this->isInvalidateVisibility() == false )
    //    //{
    //    //	if( getVisibility() == false )
    //    //	{
    //    //		return;
    //    //	}
    //    //}
    //    //else
    //    //{
    //    //	m_cameraRevision = cameraRevision;

    //    RenderContext state;
    //    state.viewport = renderViewport;
    //    state.camera = renderCamera;
    //    state.scissor = renderScissor;
    //    state.target = renderTarget;
    //    state.debugMask = _state->debugMask;

    //    //if( this->checkVisibility( viewPort ) == true )
    //    {
    //        if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
    //        {
    //            this->_render( &state );
    //        }

    //        this->renderChild_( &state );
    //    }
    //    //}

    //    if( m_renderTarget != nullptr )
    //    {
    //        this->_renderTarget( _state );
    //    }

    //    if( _state->debugMask != 0 )
    //    {
    //        if( this->isLocalHide() == false && this->isPersonalTransparent() == false )
    //        {
    //            this->_debugRender( &state );
    //        }
    //    }
    //}
    ////////////////////////////////////////////////////////////////////////////
    //void Node::_setHide( bool _value )
    //{
    //    (void)_value;

    //    m_invalidateRendering = true;
    //}    
    //////////////////////////////////////////////////////////////////////////
    void Node::_invalidateWorldMatrix()
    {
        this->invalidateBoundingBox();
        //invalidateBoundingBox();add
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getAffectorableUpdatableMode() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getAffectorableUpdatableLeafDeep() const
    {
        uint32_t deep = this->getLeafDeep();

        return deep;
    }
    //////////////////////////////////////////////////////////////////////////
    PickerTrapInterfacePtr Node::getPickerTrap()
    {
        return nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////
    //void Node::updateRendering_() const
    //{
    //    m_invalidateRendering = false;

    //    if( this->isCompile() == false )
    //    {
    //        m_rendering = false;
    //    }
    //    else if( this->isActivate() == false )
    //    {
    //        m_rendering = false;
    //    }
    //    else if( this->isHide() == true )
    //    {
    //        m_rendering = false;
    //    }
    //    else if( this->isLocalTransparent() == true )
    //    {
    //        m_rendering = false;
    //    }
    //    else
    //    {
    //        m_rendering = true;
    //    }
    //}
}
