#include "Kernel/Node.h"

#include "Interface/TimelineServiceInterface.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/IntrusivePtrScope.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/NodePickerHierarchy.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Node::Node()
        : m_state( EN_STATE_ENABLE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Node::~Node()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::dispose()
    {
        m_state |= EN_STATE_DISPOSE;

        this->removeFromParent();
        this->disable();
        this->release();

        this->_dispose();

        this->removeChildren( []( const NodePtr & )
        {} );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        this->unwrap();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::disposeAll()
    {
        this->foreachChildrenSlug( []( const NodePtr & _node )
        {
            _node->disposeAll();
        } );

        this->dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_dispose()
    {
        EventationInterface * eventation = this->getEventation();

        if( eventation != nullptr )
        {
            eventation->removeEvents();
        }

        this->clearAffectorHub();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_destroy()
    {
        MENGINE_ASSERTION_FATAL( this->isActivate() == false );

        this->release();

        this->clearAffectorHub();

        this->removeChildren( []( const NodePtr & )
        {} );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        this->unwrap();
#endif
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

        if( this->isActivate() == true )
        {
            return true;
        }

        if( this->_activate() == false )
        {
            this->release();

            return false;
        }

        IntrusivePtrScope ankh( this );

        m_state |= EN_STATE_ACTIVE;

        if( this->foreachChildrenSlugBreak( []( const NodePtr & _child )
        {
            bool result = _child->activate();

            return result;
        } ) == false )
        {
            m_state &= ~EN_STATE_ACTIVE;

            this->release();

            return false;
        }

        if( this->isActivate() == false )
        {
            this->release();

            return false;
        }

        m_state |= EN_STATE_AFTER_ACTIVE;

        if( this->_afterActivate() == false )
        {
            m_state &= ~(EN_STATE_AFTER_ACTIVE | EN_STATE_ACTIVE);

            this->release();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::deactivate()
    {
        if( this->isActivate() == false )
        {
            return;
        }

        if( this->isDeactivating() == true )
        {
            return;
        }

        IntrusivePtrScope ankh( this );

        m_state |= EN_STATE_DEACTIVATING;

        if( this->isAfterActive() == false )
        {
            LOGGER_ERROR( "node '%s' invalid deactivate in 'activate state'"
                , this->getName().c_str()
            );
        }

        m_state &= ~EN_STATE_AFTER_ACTIVE;

        this->_deactivate();

        this->foreachChildrenSlug( []( const NodePtr & _child )
        {
            _child->deactivate();
        } );

        m_state &= ~(EN_STATE_ACTIVE | EN_STATE_DEACTIVATING);

        this->_afterDeactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::enable()
    {
        if( this->isActivate() == true )
        {
            return true;
        }

        m_state |= EN_STATE_ENABLE;

        Node * parent = this->getParent();

        if( parent != nullptr )
        {
            if( parent->isActivate() == false )
            {
                return true;
            }
        }
        else
        {
            return true;
        }

        IntrusivePtrScope ankh( this );

        if( this->activate() == false )
        {
            m_state &= ~EN_STATE_ENABLE;

            return false;
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->updatePickers();
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

        m_state |= EN_STATE_ENABLE;

        Node * parent = this->getParent();

        if( parent != nullptr )
        {
            if( parent->isActivate() == false )
            {
                return true;
            }
        }

        IntrusivePtrScope ankh( this );

        if( this->activate() == false )
        {
            m_state &= ~EN_STATE_ENABLE;

            return false;
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->updatePickers();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::disable()
    {
        IntrusivePtrScope ankh( this );

        this->deactivate();

        m_state &= ~EN_STATE_ENABLE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeRelationRender_()
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->removeRelationRender();
        }
        else
        {
            Node * parent = this->getParent();

            if( Helper::hasNodeRenderInheritance( parent ) == true )
            {
                this->foreachRenderCloseChildren_( []( RenderInterface * _render )
                {
                    _render->removeRelationRender();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshRenderRelation_( Node * _parent )
    {
        Node * nodeRenderParent;
        RenderInterface * renderParent = Helper::getNodeRenderInheritance( _parent, &nodeRenderParent );

        if( renderParent != nullptr )
        {
            renderParent->clearRenderChildren();

            nodeRenderParent->foreachRenderCloseChildren_( [renderParent]( RenderInterface * _render )
            {
                _render->setRelationRender( renderParent );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshPickerRelation_( Node * _parent )
    {
        Node * nodePickerParent;
        PickerInterface * pickerParent = Helper::getNodePickerInheritance( _parent, &nodePickerParent );

        if( pickerParent != nullptr )
        {
            pickerParent->clearPickerChildren();

            nodePickerParent->foreachPickerCloseChildren_( [pickerParent]( PickerInterface * _picker )
            {
                _picker->setRelationPicker( pickerParent );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::findUniqueChild( UniqueId _uniqueIdentity ) const
    {
        NodePtr found;

        this->foreachChildrenUnslugBreak( [&found, _uniqueIdentity]( const NodePtr & _child )
        {
            UniqueId childUID = _child->getUniqueIdentity();

            if( childUID != _uniqueIdentity )
            {
                return true;
            }

            found = _child;

            return false;
        } );

        return found;
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
            picker->removeRelationPicker();
        }
        else
        {
            Node * parent = this->getParent();

            if( Helper::hasNodePickerInheritance( parent ) == true )
            {
                this->foreachPickerCloseChildren_( []( PickerInterface * _childPicker )
                {
                    _childPicker->removeRelationPicker();
                } );
            }
        }
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

        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_visitor]( const NodePtr & _child )
        {
            _child->visitChildren( _visitor );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::freeze( bool _value )
    {
        if( this->isFreeze() == _value )
        {
            return;
        }
        
        if( _value == true )
        {
            m_state |= EN_STATE_FREEZE;
        }
        else
        {
            m_state &= ~EN_STATE_FREEZE;
        }

        this->_freeze( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_freeze( bool _value )
    {
        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_value]( const NodePtr & _child )
        {
            _child->freeze( _value );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::_activate()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::_afterActivate()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            if( updation->activate( EUM_NODE_BASE, deep ) == false )
            {
                return false;
            }
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

        return true;
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

        if( this->availableAffectorHub() == true )
        {
            const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

            affectorHub->stopAllAffectors();
            this->clearAffectorHub();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_afterDeactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::compile()
    {
        if( Compilable::compile() == false )
        {
            LOGGER_ERROR( "node '%s' type '%s' not compiled"
                , this->getName().c_str()
                , this->getType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::release()
    {
        if( this->isActivate() == true )
        {
            this->deactivate();
        }

        this->foreachChildrenSlug( []( const NodePtr & _child )
        {
            _child->release();
        } );

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_recompile()
    {
        if( this->isEnable() == false )
        {
            return;
        }

        if( this->isActivate() == true )
        {
            return;
        }

        this->activate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_uncompile()
    {
        if( this->isEnable() == false )
        {
            return;
        }

        if( this->isActivate() == false )
        {
            return;
        }

        this->deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchySetParent( Node * _newParent )
    {
        TransformationInterface * transformation = this->getTransformation();

        if( transformation != nullptr )
        {
            TransformationInterface * newParentTransformation = _newParent->getTransformation();

            transformation->setRelationTransformation( newParentTransformation );
        }

        this->refreshRenderRelation_( _newParent );
        this->refreshPickerRelation_( _newParent );

        UpdationInterface * updation = this->getUpdation();

        if( _newParent != nullptr && updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->replace( deep );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRemoveParent( Node * _oldParent )
    {
        MENGINE_UNUSED( _oldParent );

        TransformationInterface * transformation = this->getTransformation();

        if( transformation != nullptr )
        {
            transformation->removeRelationTransformation();
        }

        this->removeRelationRender_();
        this->removeRelationPicker_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRefreshChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        this->refreshRenderRelation_( this );
        this->refreshPickerRelation_( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyChangeParent( Node * _oldParent, Node * _newParent )
    {
        MENGINE_UNUSED( _oldParent );
        MENGINE_UNUSED( _newParent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyAddChild( const NodePtr & _node )
    {
        TransformationInterface * transformation = _node->getTransformation();

        if( transformation != nullptr )
        {
            transformation->invalidateWorldMatrix();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRemoveChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    AffectorHubProviderInterface * Node::getAffectorHubProvider()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    EUpdateMode Node::getAffectorableUpdatableMode() const
    {
        return EUM_NODE_AFFECTOR;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getAffectorableUpdatableLeafDeep() const
    {
        uint32_t deep = this->getLeafDeep();

        return deep;
    }
    //////////////////////////////////////////////////////////////////////////
}
