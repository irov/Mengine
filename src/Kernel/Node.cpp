#include "Kernel/Node.h"

#include "Interface/TimelineServiceInterface.h"
#include "Interface/PickerServiceInterface.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/IntrusivePtrScope.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/NodePickerHierarchy.h"
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

        IntrusivePtrScope ankh( this );

        m_active = true;

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
                    NodePtr children( *it );

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
            LOGGER_ERROR( "node '%s' invalid deactivate in 'activate state'"
                , this->getName().c_str()
            );
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
                    NodePtr children( *it );

                    it.next_shuffle();

                    children->deactivate();
                }
            }
        }

        m_active = false;
        m_deactivating = false;

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
        IntrusivePtrScope ankh( this );

        this->deactivate();

        m_enable = false;
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
    void Node::removeRelationRender_()
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * oldRenderParent = render->getRelationRender();

            if( oldRenderParent != nullptr )
            {
                render->removeRelationRender();
            }
        }
        else
        {
            RenderInterface * oldRenderParent = Helper::getNodeRenderInheritance( m_parent );

            if( oldRenderParent != nullptr )
            {
                this->foreachRenderCloseChildren_( []( RenderInterface * _render )
                {
                    _render->removeRelationRender();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setRelationRender_( Node * _parent )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * oldRenderParent = render->getRelationRender();
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                if( newRenderParent != nullptr )
                {
                    render->setRelationRender( newRenderParent );
                }
                else
                {
                    render->removeRelationRender();
                }
            }
        }
        else
        {
            RenderInterface * oldRenderParent = Helper::getNodeRenderInheritance( m_parent );
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                if( newRenderParent != nullptr )
                {
                    this->foreachRenderCloseChildren_( [newRenderParent]( RenderInterface * _render )
                    {
                        _render->setRelationRender( newRenderParent );
                    } );
                }
                else
                {
                    this->foreachRenderCloseChildren_( []( RenderInterface * _render )
                    {
                        _render->removeRelationRender();
                    } );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setRelationRenderFront_( Node * _parent )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * oldRenderParent = render->getRelationRender();
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                if( newRenderParent != nullptr )
                {
                    render->setRelationRenderFront( newRenderParent );
                }
                else
                {
                    render->removeRelationRender();
                }
            }
        }
        else
        {
            RenderInterface * oldRenderParent = Helper::getNodeRenderInheritance( m_parent );
            RenderInterface * newRenderParent = Helper::getNodeRenderInheritance( _parent );

            if( oldRenderParent != newRenderParent )
            {
                if( newRenderParent != nullptr )
                {
                    this->foreachRenderReverseCloseChildren_( [newRenderParent]( RenderInterface * _render )
                    {
                        _render->setRelationRenderFront( newRenderParent );
                    } );
                }
                else
                {
                    this->foreachRenderReverseCloseChildren_( []( RenderInterface * _render )
                    {
                        _render->removeRelationRender();
                    } );
                }

            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildRenderFront_( const NodePtr & _child )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * childRender = _child->getRender();

            if( childRender != nullptr )
            {
                render->moveRelationRenderFront( childRender );
            }
            else
            {
                _child->foreachRenderReverseCloseChildren_( [render]( RenderInterface * _childRender )
                {
                    render->moveRelationRenderFront( _childRender );
                } );
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildRenderMiddle_( const NodePtr & _after, const NodePtr & _child )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * afterRender = _after->getRender();
            RenderInterface * childRender = _child->getRender();

            if( afterRender != nullptr && childRender != nullptr )
            {
                render->moveRelationRenderMiddle( afterRender, childRender );
            }
            else
            {
                MENGINE_ASSERTION_NOT_IMPLEMENTED();
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildRenderBack_( const NodePtr & _child )
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            RenderInterface * childRender = _child->getRender();

            if( childRender != nullptr )
            {
                render->moveRelationRenderBack( childRender );
            }
            else
            {
                _child->foreachRenderCloseChildren_( [render]( RenderInterface * _childRender )
                {
                    render->moveRelationRenderBack( _childRender );
                } );
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeParent_()
    {
        this->removeRelationTransformation();
        this->removeRelationRender_();
        this->removeRelationPicker_();

        Node * oldparent = m_parent;
        m_parent = nullptr;

        this->_changeParent( oldparent, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setParent_( Node * _parent, ENodeChildInsertMode _mode )
    {
        switch( _mode )
        {
        case ENCI_FRONT:
            {
                this->setRelationTransformation( _parent );
                this->setRelationRenderFront_( _parent );
                this->setRelationPickerFront_( _parent );
            }break;
        case ENCI_MIDDLE:
            {
                this->setRelationTransformation( _parent );
                this->setRelationRender_( _parent );
                this->setRelationPickerBack_( _parent );
            }break;
        case ENCI_BACK:
            {
                this->setRelationTransformation( _parent );
                this->setRelationRender_( _parent );
                this->setRelationPickerBack_( _parent );
            }break;
        }        

        UpdationInterface * updation = this->getUpdation();

        if( _parent != nullptr && updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->replace( deep );
        }

        Node * oldparent = m_parent;
        m_parent = _parent;

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
            NodePtr node( *it );

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
    void Node::removeChildren( const LambdaRemoveChildren & _lambda )
    {
        if( m_children.empty() == true )
        {
            return;
        }

        for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
        {
            NodePtr node( *it );

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

        bool result = m_parent->removeChild( NodePtr::from( this ) );

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
        MENGINE_ASSERTION_RETURN_VOID( _node != nullptr, "node '%s' invalid add child NULL node"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_RETURN_VOID( _node != this, "node '%s' invalid self child node"
            , this->getName().c_str()
        );

        this->addChild_( m_children.end(), _node, ENCI_BACK );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addChildFront( const NodePtr & _node )
    {
        MENGINE_ASSERTION( _node != nullptr, "node '%s' invalid add front child NULL node"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_RETURN_VOID( _node != this, "node '%s' invalid self child node"
            , this->getName().c_str()
        );

        this->addChild_( m_children.begin(), _node, ENCI_FRONT );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::addChildAfter( const NodePtr & _node, const NodePtr & _after )
    {
        MENGINE_ASSERTION( _node != nullptr, "node '%s' invalid add child NULL node (node)"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION( _after != nullptr, "node '%s' invalid add after NULL node (node)"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION( _node != _after, "node '%s' invalid add child '%s' is equal after '%s' (node)"
            , this->getName().c_str()
            , _node->getName().c_str()
            , _after->getName().c_str()
        );

        MENGINE_ASSERTION( stdex::helper::intrusive_find( m_children.begin(), m_children.end(), _after ) != m_children.end(), "node '%s' after is not child"
            , this->getName().c_str()
        );

        IntrusiveSlugListNodeChild::iterator it_after( _after );

        this->addChild_( it_after, _node, ENCI_MIDDLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _child, ENodeChildInsertMode _mode )
    {
        IntrusivePtrScope ankh( this );

        Node * child_parent = _child->getParent();

        if( child_parent == this )
        {
            if( *_insert == _child )
            {
                return;
            }

            const NodePtr & frontNode = m_children.front();
            const NodePtr & backNode = m_children.back();

            switch( _mode )
            {
            case ENCI_FRONT:
                {
                    if( frontNode == _child )
                    {
                        return;
                    }
                }break;
            case ENCI_BACK:
                {
                    if( backNode == _child )
                    {
                        return;
                    }
                }break;
            default:
                break;
            }

            this->eraseChild_( _child );

            this->insertChild_( _insert, _child );

            switch( _mode )
            {
            case ENCI_FRONT:
                {
                    this->moveChildRenderFront_( _child );
                    this->moveChildPickerFront_( _child );
                }break;
            case ENCI_MIDDLE:
                {
                    this->moveChildRenderMiddle_( *_insert, _child );
                    this->moveChildPickerMiddle_( *_insert, _child );
                }break;
            case ENCI_BACK:
                {
                    this->moveChildRenderBack_( _child );
                    this->moveChildPickerBack_( _child );
                }break;
            }
        }
        else
        {
            if( child_parent != nullptr )
            {
                child_parent->eraseChild_( _child );
            }

            this->insertChild_( _insert, _child );

            _child->setParent_( this, _mode );
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
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::insertChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _node )
    {
        m_children.insert( _insert, _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::eraseChild_( const NodePtr & _node )
    {
        m_children.remove( _node );
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
            NodePtr child( *it );

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
            NodePtr child( *it );

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachChildrenReverse( const LambdaNode & _lambda ) const
    {
        for( IntrusiveSlugListNodeChild::const_reverse_iterator
            it = m_children.rbegin(),
            it_end = m_children.rend();
            it != it_end;
            ++it )
        {
            NodePtr child( *it );

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
            NodePtr child( *it );

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
            NodePtr child( *it );

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
    void Node::foreachRenderCloseChildren_( const LambdaRenderCloseChildren & _lambda )
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
                _child->foreachRenderCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachRenderReverseCloseChildren_( const LambdaRenderCloseChildren & _lambda )
    {
        this->foreachChildrenReverse( [&_lambda]( const NodePtr & _child )
        {
            RenderInterface * render = _child->getRender();

            if( render != nullptr )
            {
                _lambda( render );
            }
            else
            {
                _child->foreachRenderReverseCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeRelationPicker_()
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * oldParent = picker->getRelationPicker();

            if( oldParent != nullptr )
            {
                picker->removeRelationPicker();
            }
        }
        else
        {
            PickerInterface * oldParent = Helper::getNodePickerInheritance( m_parent );

            if( oldParent != nullptr )
            {
                this->foreachPickerCloseChildren_( []( PickerInterface * _childPicker )
                {
                    _childPicker->removeRelationPicker();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setRelationPickerBack_( Node * _parent )
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * oldParent = picker->getRelationPicker();
            PickerInterface * newParent = Helper::getNodePickerInheritance( _parent );

            if( oldParent != newParent )
            {
                if( newParent != nullptr )
                {
                    picker->setRelationPickerBack( newParent );
                }
                else
                {
                    picker->removeRelationPicker();
                }
            }
        }
        else
        {
            PickerInterface * oldParent = Helper::getNodePickerInheritance( m_parent );
            PickerInterface * newParent = Helper::getNodePickerInheritance( _parent );

            if( oldParent != newParent )
            {
                if( newParent != nullptr )
                {
                    this->foreachPickerCloseChildren_( [newParent]( PickerInterface * _childPicker )
                    {
                        _childPicker->setRelationPickerBack( newParent );
                    } );
                }
                else
                {
                    this->foreachPickerCloseChildren_( []( PickerInterface * _childPicker )
                    {
                        _childPicker->removeRelationPicker();
                    } );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setRelationPickerFront_( Node * _parent )
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * oldParent = picker->getRelationPicker();
            PickerInterface * newParent = Helper::getNodePickerInheritance( _parent );

            if( oldParent != newParent )
            {
                if( newParent != nullptr )
                {
                    picker->setRelationPickerFront( newParent );
                }
                else
                {
                    picker->removeRelationPicker();
                }
            }
        }
        else
        {
            PickerInterface * oldParent = Helper::getNodePickerInheritance( m_parent );
            PickerInterface * newParent = Helper::getNodePickerInheritance( _parent );

            if( oldParent != newParent )
            {
                if( newParent != nullptr )
                {
                    this->foreachPickerReverseCloseChildren_( [newParent]( PickerInterface * _childPicker )
                    {
                        _childPicker->setRelationPickerFront( newParent );
                    } );
                }
                else
                {
                    this->foreachPickerReverseCloseChildren_( []( PickerInterface * _childPicker )
                    {
                        _childPicker->removeRelationPicker();
                    } );
                }

            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildPickerFront_( const NodePtr & _child )
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * childPicker = _child->getPicker();

            if( childPicker != nullptr )
            {
                picker->moveRelationPickerFront( childPicker );
            }
            else
            {
                this->foreachPickerReverseCloseChildren_( [picker]( PickerInterface * _childPicker )
                {
                    picker->moveRelationPickerFront( _childPicker );
                } );
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildPickerMiddle_( const NodePtr & _after, const NodePtr & _child )
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * afterPicker = _after->getPicker();
            PickerInterface * childPicker = _child->getPicker();

            if( afterPicker != nullptr && childPicker != nullptr )
            {
                picker->moveRelationPickerMiddle( afterPicker, childPicker );
            }
            else
            {
                PickerInterface * afterSiblingPicker = this->getPickerSiblingPrev_( _after );

                if( afterSiblingPicker != nullptr )
                {
                    _child->foreachPickerReverseCloseChildren_( [afterSiblingPicker, picker]( PickerInterface * _childPicker )
                    {
                        picker->moveRelationPickerMiddle( afterSiblingPicker, _childPicker );
                    } );
                }
                else
                {
                    _child->foreachPickerReverseCloseChildren_( [picker]( PickerInterface * _childPicker )
                    {
                        picker->moveRelationPickerFront( _childPicker );
                    } );
                }
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::moveChildPickerBack_( const NodePtr & _child )
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            PickerInterface * childPicker = _child->getPicker();

            if( childPicker != nullptr )
            {
                picker->moveRelationPickerBack( childPicker );
            }
            else
            {
                _child->foreachPickerCloseChildren_( [picker]( PickerInterface * _childPicker )
                {
                    picker->moveRelationPickerBack( _childPicker );
                } );
            }
        }
        else
        {
            MENGINE_ASSERTION_NOT_IMPLEMENTED();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PickerInterface * Node::getPickerSiblingPrev_( const NodePtr & _after ) const
    {
        NodePtr siblingNode = nullptr;
        PickerInterface * siblingPicker = nullptr;

        this->foreachChildrenUnslugBreak( [&siblingNode, &siblingPicker, &_after]( const NodePtr & _child )
        {
            if( _after == _child )
            {
                return false;
            }

            PickerInterface * picker = _child->getPicker();

            if( picker == nullptr )
            {
                return true;
            }

            siblingNode = _child;
            siblingPicker = picker;

            return true;
        } );

        return siblingPicker;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachPickerCloseChildren_( const LambdaPickerCloseChildren & _lambda )
    {
        this->foreachChildren( [&_lambda]( const NodePtr & _child )
        {
            PickerInterface * picker = _child->getPicker();

            if( picker != nullptr )
            {
                _lambda( picker );
            }
            else
            {
                _child->foreachPickerCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachPickerReverseCloseChildren_( const LambdaPickerCloseChildren & _lambda )
    {
        this->foreachChildrenReverse( [&_lambda]( const NodePtr & _child )
        {
            PickerInterface * picker = _child->getPicker();

            if( picker != nullptr )
            {
                _lambda( picker );
            }
            else
            {
                _child->foreachPickerReverseCloseChildren_( _lambda );
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

        IntrusivePtrScope ankh( this );

        NodePtr single_child = m_children.single();

        if( single_child != nullptr )
        {
            single_child->visitChildren( _visitor );
        }
        else
        {
            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                NodePtr children( *it );

                it.next_shuffle();

                children->visitChildren( _visitor );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::removeChild( const NodePtr & _node )
    {
        MENGINE_ASSERTION_RETURN( stdex::helper::intrusive_has( m_children.begin(), m_children.end(), _node ) == true, false, "node '%s' not found children '%s'"
            , this->getName().c_str()
            , _node->getName().c_str()
        );

        MENGINE_ASSERTION_RETURN( _node != this, false, "node '%s' invalid self child node"
            , this->getName().c_str()
        );

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
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _recursion );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::getSiblingPrev() const
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }

        IntrusiveSlugListNodeChild & parent_children = parent->getChildren();

        IntrusiveSlugListNodeChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), NodePtr( this ) );

        if( it_found == parent_children.begin() )
        {
            return nullptr;
        }

        --it_found;

        NodePtr prev_node = *it_found;

        return prev_node;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::getSiblingNext() const
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }

        IntrusiveSlugListNodeChild & parent_children = parent->getChildren();

        IntrusiveSlugListNodeChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), NodePtr( this ) );

        ++it_found;

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
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _recursive );

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
        MENGINE_UNUSED( _oldParent );
        MENGINE_UNUSED( _newParent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_addChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_removeChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

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
            IntrusivePtrScope ankh( this );

            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                NodePtr node( *it );

                it.next_shuffle();

                node->freeze( _value );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setSpeedFactor( float _speedFactor )
    {
        MENGINE_UNUSED( _speedFactor );

        //TODO: REMOVE
    }
    //////////////////////////////////////////////////////////////////////////
    float Node::getSpeedFactor() const
    {
        return 0.f;
    }
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

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->setPickerEnable( true );
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

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->setPickerEnable( false );
        }

        Affectorable::stopAllAffectors();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_afterDeactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::compile()
    {
        bool result = Compilable::compile();

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
                    NodePtr node( *it );

                    it.next_shuffle();

                    node->release();
                }
            }
        }

        Compilable::release();
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
    void Node::_uncompile()
    {
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
    void Node::_invalidateWorldMatrix()
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->invalidateBoundingBox();
        }
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
}
